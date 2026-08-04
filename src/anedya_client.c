#include "anedya.h"
#include "anedya_certs.h"
#include "anedya_json_parse.h"
#include "anedya_op_commands.h"
#include "anedya_operations.h"
#include "string.h"
#include "sys/time.h" // TODO: Remove time header dependency
#include <stdio.h>

anedya_err_t anedya_client_init(anedya_config_t *config,
                                anedya_client_t *client) {
  client->config = config;

  // Initialize the interface
  anedya_err_t err = _anedya_interface_init(client);
  if (err != ANEDYA_OK) {
    return err;
  }

#ifdef ANEDYA_ENABLE_DYNAMIC_ALLOCATION
  client->tx_buffer = anedya_interface_allocate_memory(config->tx_buffer_size);
  if (client->tx_buffer == NULL) {
    return ANEDYA_ERR_NO_MEMORY;
  }
  client->rx_buffer = anedya_interface_allocate_memory(config->rx_buffer_size);
  if (client->rx_buffer == NULL) {
    return ANEDYA_ERR_NO_MEMORY;
  }
#ifdef ANEDYA_ENABLE_DEVICE_LOGS
  client->log_buffer = anedya_interface_allocate_memory(
      config->log_buffer_size * config->log_batch_size);
  if (client->log_buffer == NULL) {
    return ANEDYA_ERR_NO_MEMORY;
  }
#endif
#endif

// Compute the broker url
#ifdef ANEDYA_CONNECTION_METHOD_MQTT
  char url[100];
  sprintf(url, "mqtt.%s.anedya.io", config->region);
  strcpy(client->broker_url, url);
  anedya_mqtt_client_handle_t handle = _anedya_interface_mqtt_init(
      client, client->broker_url, client->config->_device_id_str,
      client->config->connection_key);
  client->mqtt_client = handle;
  client->_message_handler = _anedya_message_handler;
  client->_anedya_on_connect_handler = _anedya_on_connect_handler;
  client->_anedya_on_disconnect_handler = _anedya_on_disconnect_handler;
  // Initialize txn store
  err = _anedya_txn_store_init(&client->txn_store);
  if (err != ANEDYA_OK) {
    return err;
  }
#endif

#ifdef ANEDYA_CONNECTION_METHOD_HTTP
  // Build REST base URL: device.<region>.anedya.io
  char http_url[100];
  sprintf(http_url, "device.%s.anedya.io", config->region);
  strcpy(client->http_base_url, http_url);
  // HTTP is stateless - mark as connected immediately
  client->is_connected = 1;
  // Initialize txn store (still used to track in-flight requests)
  err = _anedya_txn_store_init(&client->txn_store);
  if (err != ANEDYA_OK) {
    return err;
  }
#endif

#ifdef ANEDYA_CONNECTION_METHOD_MQTT
  char topic_prefix[100];
  strcpy(topic_prefix, "$anedya/device/");
  strcat(topic_prefix, config->_device_id_str);
  strcat(topic_prefix, "/");

  strcpy(client->_message_topics[0], topic_prefix);
  strcat(client->_message_topics[0], "response");

  strcpy(client->_message_topics[1], topic_prefix);
  strcat(client->_message_topics[1], "errors");

  strcpy(client->_message_topics[2], topic_prefix);
  strcat(client->_message_topics[2], "commands");

  strcpy(client->_message_topics[3], topic_prefix);
  strcat(client->_message_topics[3], "valuestore/updates/json");
#endif

  return ANEDYA_OK;
}

#ifdef ANEDYA_CONNECTION_METHOD_MQTT
anedya_err_t anedya_client_connect(anedya_client_t *client) {
  anedya_err_t err;
  err = anedya_interface_mqtt_connect(client->mqtt_client);
  if (err != ANEDYA_OK) {
    return err;
  }
  return ANEDYA_OK;
}

anedya_err_t anedya_client_disconnect(anedya_client_t *client) {
  anedya_err_t err;
  // Set is_connected to 0 to signify intentional shutdown
  client->is_connected = 0;
  err = anedya_interface_mqtt_disconnect(client->mqtt_client);
  if (err != ANEDYA_OK) {
    return err;
  }
  return ANEDYA_OK;
}

anedya_err_t anedya_client_destroy(anedya_client_t *client) {
  anedya_err_t err;
  err = anedya_interface_mqtt_destroy(client->mqtt_client);
  if (err != ANEDYA_OK) {
    return err;
  }
  return ANEDYA_OK;
}

#endif

#ifdef ANEDYA_CONNECTION_METHOD_HTTP
anedya_err_t anedya_client_connect(anedya_client_t *client) {
  /* HTTP is stateless – no persistent TCP connection needed.
   * Just mark as connected so operation functions can proceed. */
  client->is_connected = 1;
  return ANEDYA_OK;
}

anedya_err_t anedya_client_disconnect(anedya_client_t *client) {
  client->is_connected = 0;
  return ANEDYA_OK;
}
#endif

anedya_err_t _anedya_txn_store_init(anedya_txn_store_t *store) {

  for (int i = 0; i < ANEDYA_MAX_CONCURRENT_TXN; i++) {
    store->txn_slot_free[i] = 1;
    store->txns[i] = NULL;
  }
  store->_lock = 0;
  return ANEDYA_OK;
}

anedya_err_t _anedya_txn_store_aquire_slot(anedya_txn_store_t *store,
                                           anedya_txn_t *txn) {
  // Aquire lock
  if (store->_lock == 1) {
    // printf("Lock failed while aquiring\r\n");
    return ANEDYA_ERR_LOCK_FAILED;
  }
  // Take lock
  store->_lock = 1;
  // Loop through the slots, and aquire the first available one
  for (int i = 0; i < ANEDYA_MAX_CONCURRENT_TXN; i++) {
    int64_t now = time(NULL);
    if (store->txn_slot_free[i] == 0 && (now - store->aquired_time[i]) > 30) {
      // TXN has timed out. Free the aquired slot
      store->txn_slot_free[i] = 1;
      store->txns[i] = NULL;
      // printf("Slot timed out: %d", i + 1);
    }
    if (store->txn_slot_free[i] == 1) {
      store->txn_slot_free[i] = 0;
      store->txns[i] = txn;
      store->aquired_time[i] = (int64_t)time(NULL);
      txn->desc = i + 1;
      store->_lock = 0;
      // printf("Aquired slot: %d\r\n", txn->desc);
      return ANEDYA_OK;
    }
  }
  // Release the lock
  store->_lock = 0;
  return ANEDYA_ERR_MAX_TXN_EXCEEDED;
}

anedya_err_t _anedya_txn_store_release_slot(anedya_txn_store_t *store,
                                            anedya_txn_t *txn) {
  // Aquire lock
  if (store->_lock == 1) {
    // printf("Lock failed while releasing\r\n");
    return ANEDYA_ERR_LOCK_FAILED;
  }
  // Take lock
  store->_lock = 1;
  store->txn_slot_free[txn->desc - 1] = 1;
  store->txns[txn->desc - 1] = NULL;
  // printf("Released slot: %d\r\n", txn->desc);
  //  Release the lock
  store->_lock = 0;
  return ANEDYA_OK;
}

#ifdef ANEDYA_CONNECTION_METHOD_MQTT
void _anedya_message_handler(anedya_client_t *cl, char *topic, int topic_len,
                             char *payload, int payload_len) {
  // Just received the message, now determine for which topic this message is
  // delivered
  int i = 0;
  for (i = 0; i < 4; i++) {
    if (strncmp(topic, cl->_message_topics[i],
                strlen(cl->_message_topics[i]) - 1) == 0) {
      break;
    }
  }
  switch (i) {
  case 0:
    _anedya_handle_txn_response(cl, payload, payload_len, 0);
    break;
  case 1:
    _anedya_handle_txn_response(cl, payload, payload_len, 1);
    break;
  case 2:
    _anedya_handle_event(cl, payload, payload_len, 2);
    break;
  case 3:
    _anedya_handle_event(cl, payload, payload_len, 3);
    break;
  }
}

void _anedya_on_connect_handler(anedya_client_t *client) {
  // Client just got connected to the broker, now subscribe to the topics
  anedya_interface_mqtt_subscribe(client->mqtt_client,
                                  client->_message_topics[0],
                                  strlen(client->_message_topics[0]), 0);
  anedya_interface_mqtt_subscribe(client->mqtt_client,
                                  client->_message_topics[1],
                                  strlen(client->_message_topics[1]), 0);
  anedya_interface_mqtt_subscribe(client->mqtt_client,
                                  client->_message_topics[2],
                                  strlen(client->_message_topics[2]), 0);
  anedya_interface_mqtt_subscribe(client->mqtt_client,
                                  client->_message_topics[3],
                                  strlen(client->_message_topics[3]), 0);
  client->is_connected = 1;
  if (client->config->on_connect != NULL) {
    client->config->on_connect(client->config->on_connect_ctx);
  }
  return;
}

void _anedya_on_disconnect_handler(anedya_client_t *client) {
  // Call the callback
  if (client->config->on_disconnect != NULL) {
    client->config->on_disconnect(client->config->on_disconnect_ctx);
  }

  if (client->is_connected == 1) {
    // This means the flow is coming from unintentional connection close
    // Process retry logic
    client->is_connected = 0;
    anedya_interface_mqtt_connect(client->mqtt_client);
    // TODO: Implement retry logic
  }
  return;
}
#endif /* ANEDYA_CONNECTION_METHOD_MQTT */

#ifdef ANEDYA_CONNECTION_METHOD_HTTP
void _anedya_handle_http_txn_response(anedya_client_t *cl, char *payload,
                                      int payload_len, anedya_txn_t *txn) {
  if (payload == NULL || payload_len <= 0)
    return;

  txn->_rx_len = payload_len + 1;
  if (txn->_rx_len > ANEDYA_RX_BUFFER_SIZE) {
    txn->_op_err = ANEDYA_ERR_RX_BUFFER_OVERFLOW;
    txn->is_complete = true;
    txn->is_success = false;
    _anedya_txn_complete(cl, txn);
    return;
  }

  memcpy(txn->_rxbody, payload, payload_len);
  txn->_rxbody[payload_len] = '\0';

  switch (txn->_op) {
  case ANEDYA_OP_BIND_DEVICE:
    _anedya_device_handle_generic_resp(cl, txn);
    break;
  case ANEDYA_OP_HEARTBEAT:
    _anedya_device_handle_generic_resp(cl, txn);
    break;
  case ANEDYA_OP_OTA_NEXT:
    _anedya_op_ota_next_resp(cl, txn);
    break;
  case ANEDYA_OP_ONGOING_OTA:
    _anedya_op_ongoing_ota_resp(cl, txn);
    break;
  case ANEDYA_OP_SUBMIT_DATA:
    _anedya_device_handle_generic_resp(cl, txn);
    break;
  case ANEDYA_OP_VALUESTORE_SET:
    _anedya_device_handle_generic_resp(cl, txn);
    break;
  case ANEDYA_OP_SUBMIT_EVENT:
    _anedya_device_handle_generic_resp(cl, txn);
    break;
  case ANEDYA_OP_CMD_UPDATE_STATUS:
    _anedya_device_handle_generic_resp(cl, txn);
    break;
  case ANEDYA_OP_SUBMIT_LOG:
    _anedya_device_handle_generic_resp(cl, txn);
    break;
  case ANEDYA_OP_VALUESTORE_GET:
    _anedya_op_valuestore_handle_get_resp(cl, txn);
    break;
  case ANEDYA_OP_VALUESTORE_GET_LIST:
    _anedya_op_valuestore_handle_list_obj_resp(cl, txn);
    break;
  case ANEDYA_OP_VALUESTORE_DELETE:
    _anedya_device_handle_generic_resp(cl, txn);
    break;
  case ANEDYA_OP_CMD_GET_LIST:
    _anedya_op_command_handle_list_resp(cl, txn);
    break;
  case ANEDYA_OP_CMD_NEXT:
    _anedya_op_cmd_handle_next_resp(cl, txn);
    break;
  default:
    break;
  }
  _anedya_txn_complete(cl, txn);
}
#endif

void _anedya_handle_txn_response(anedya_client_t *cl, char *payload,
                                 int payload_len, uint8_t topic) {
  // Parse the payload, and get the txn id
  // printf("Handling txn response\r\n");
  // file-scope globals (BSS section, no runtime malloc)
  static char buffer[ANEDYA_RX_BUFFER_SIZE];
  static char str[ANEDYA_RX_BUFFER_SIZE];
  if (payload_len >= ANEDYA_RX_BUFFER_SIZE) {
    _anedya_interface_std_out("Error: Payload too large for RX buffer");
    return;
  }
  int str_len = payload_len;
  memset(str, 0, ANEDYA_RX_BUFFER_SIZE);
  memset(buffer, 0, ANEDYA_RX_BUFFER_SIZE);
  memcpy(str, payload, payload_len);
  memcpy(buffer, str, str_len);
  // printf("Payload Received: %s\r\n", str);
  str[str_len] = '\0';
  buffer[str_len] = '\0';
  json_t mem[32];
  // Parse the json and get the txn id
  json_t const *json = json_create(str, mem, sizeof mem / sizeof *mem);
  if (!json) {
    _anedya_interface_std_out("Error while parsing JSON body in TXN handler");
  }
  // Get the txn idL
  json_t const *txn_id = json_getProperty(json, "reqId");
  if (!txn_id || JSON_TEXT != json_getType(txn_id)) {
    _anedya_interface_std_out(
        "Error, the reqId property is not found in the response.");
  }
  char const *txn_index = json_getValue(txn_id);
  int index = atoi(txn_index);
  // If the txn id is 0, then it is an error
  // printf("Txn id: %d\r\n", index);
  if (index == 0) {
    _anedya_interface_std_out("Error, invalid txn id");
  } else {

    // Search for the txn in the txn store
    anedya_txn_t *txn = cl->txn_store.txns[index - 1];
    if (txn == NULL) {
      return;
    } else {
      strcpy(txn->_rxbody, buffer);
    }
    txn->_rx_len = str_len + 1;
    if (txn->_rx_len > ANEDYA_RX_BUFFER_SIZE) {
      txn->_op_err = ANEDYA_ERR_RX_BUFFER_OVERFLOW;
      txn->is_complete = true;
      txn->is_success = false;
      _anedya_txn_complete(cl, txn);
      return;
    }
    // printf("Rx Body: %s", txn->_rxbody);
    //  Call the Operation handler
    switch (txn->_op) {
    case ANEDYA_OP_BIND_DEVICE:
      _anedya_device_handle_generic_resp(cl, txn);
      break;
    case ANEDYA_OP_HEARTBEAT:
      _anedya_device_handle_generic_resp(cl, txn);
      break;
    case ANEDYA_OP_OTA_NEXT:
      _anedya_op_ota_next_resp(cl, txn);
      break;
    case ANEDYA_OP_ONGOING_OTA:
      _anedya_op_ongoing_ota_resp(cl, txn);
      break;
    case ANEDYA_OP_SUBMIT_DATA:
      _anedya_device_handle_generic_resp(cl, txn);
      break;
    case ANEDYA_OP_VALUESTORE_SET:
      _anedya_device_handle_generic_resp(cl, txn);
      break;
    case ANEDYA_OP_SUBMIT_EVENT:
      _anedya_device_handle_generic_resp(cl, txn);
      break;
    case ANEDYA_OP_CMD_UPDATE_STATUS:
      _anedya_device_handle_generic_resp(cl, txn);
      break;
    case ANEDYA_OP_SUBMIT_LOG:
      _anedya_device_handle_generic_resp(cl, txn);
      break;
    case ANEDYA_OP_VALUESTORE_GET:
      _anedya_op_valuestore_handle_get_resp(cl, txn);
      break;
    case ANEDYA_OP_VALUESTORE_GET_LIST:
      _anedya_op_valuestore_handle_list_obj_resp(cl, txn);
      break;
    case ANEDYA_OP_VALUESTORE_DELETE:
      _anedya_device_handle_generic_resp(cl, txn);
      break;
    case ANEDYA_OP_CMD_GET_LIST:
      _anedya_op_command_handle_list_resp(cl, txn);
      break;
    case ANEDYA_OP_CMD_NEXT:
      _anedya_op_cmd_handle_next_resp(cl, txn);
      break;
    default:
      // Do nothing
      break;
    }
    // Mark transaction as completed
    _anedya_txn_complete(cl, txn);
  }
  // If not found, handle error
  return;
}

#ifdef ANEDYA_CONNECTION_METHOD_MQTT
void _anedya_handle_event(anedya_client_t *cl, char *payload, int payload_len,
                          uint8_t topic) {
  // A new event has been triggered
  char *buffer = malloc(payload_len + 1);
  if (!buffer) {
    _anedya_interface_std_out("OOM in _anedya_handle_event");
    return;
  }
  int buffer_len = payload_len;
  memcpy(buffer, payload, payload_len);
  buffer[payload_len] = '\0';
  switch (topic) {
  case 2:
    // Handle command
    anedya_command_obj_t cmd;
    cmd.cmd_data_type = ANEDYA_DATATYPE_UNKNOWN;
    _anedya_parse_inbound_command(buffer, buffer_len, &cmd);
    if (cl->config->event_handler != NULL) {
      cl->config->event_handler(cl, ANEDYA_EVENT_COMMAND, &cmd);
    }
    break;
  case 3:
    // Handle valuestore update
    uint8_t type = _anedya_parse_valuestore_type(buffer, buffer_len);
    switch (type) {
    case ANEDYA_VALUESTORE_TYPE_FLOAT:
      anedya_valuestore_obj_float_t float_data;
      _anedya_parse_valuestore_float(buffer, buffer_len, &float_data);
      if (cl->config->event_handler != NULL) {
        cl->config->event_handler(cl, ANEDYA_EVENT_VS_UPDATE_FLOAT,
                                  &float_data);
      }
      break;
    case ANEDYA_VALUESTORE_TYPE_STRING:
      anedya_valuestore_obj_string_t str_data;
      _anedya_parse_valuestore_string(buffer, buffer_len, &str_data);
      if (cl->config->event_handler != NULL) {
        cl->config->event_handler(cl, ANEDYA_EVENT_VS_UPDATE_STRING, &str_data);
      }
      break;
    case ANEDYA_VALUESTORE_TYPE_BOOL:
      anedya_valuestore_obj_bool_t bool_data;
      _anedya_parse_valuestore_bool(buffer, buffer_len, &bool_data);
      if (cl->config->event_handler != NULL) {
        cl->config->event_handler(cl, ANEDYA_EVENT_VS_UPDATE_BOOL, &bool_data);
      }
      break;
    case ANEDYA_VALUESTORE_TYPE_BIN:
      anedya_valuestore_obj_bin_t bin_data;
      _anedya_parse_valuestore_bin(buffer, buffer_len, &bin_data);
      if (cl->config->event_handler != NULL) {
        cl->config->event_handler(cl, ANEDYA_EVENT_VS_UPDATE_BIN, &bin_data);
      }
      break;
    default:
      break;
    }
    break;
  }
  free(buffer);
}
#endif /* ANEDYA_CONNECTION_METHOD_MQTT */