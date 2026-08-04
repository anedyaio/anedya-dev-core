#include "anedya_operations.h"

anedya_err_t anedya_op_submit_event(anedya_client_t *client, anedya_txn_t *txn,
                                    anedya_req_submit_event_t *req_config) {
// First check if client is already connected or not
#ifdef ANEDYA_CONNECTION_METHOD_MQTT
  if (client->is_connected == 0) {
    return ANEDYA_ERR_NOT_CONNECTED;
  }
#endif
  // If it is connected, then create a txn
  txn->_op = ANEDYA_OP_SUBMIT_EVENT;
  anedya_err_t err = _anedya_txn_register(client, txn);
  if (err != ANEDYA_OK) {
    return err;
  }
// Generate the JSON body
#ifdef ANEDYA_ENABLE_STATIC_ALLOCATION
  char txbuffer[ANEDYA_TX_BUFFER_SIZE];
  size_t marker = sizeof(txbuffer);
#endif
#ifdef ANEDYA_ENABLE_DYNAMIC_ALLOCATION
// TODO: Implement dynamic allocation
#endif
  char slot_number[4];
  int digitLen = snprintf(slot_number, sizeof(slot_number), "%d", txn->desc);
  char *p = anedya_json_objOpen(txbuffer, NULL, &marker);
  // Get the reqId based on slot.
#ifdef ANEDYA_CONNECTION_METHOD_MQTT
  p = anedya_json_nstr(p, "reqId", slot_number, digitLen, &marker);
#endif
  p = anedya_json_nstr(p, "eventType", req_config->event_type,
                       strlen(req_config->event_type), &marker);
  p = anedya_json_verylong(p, "timestamp", req_config->timestamp, &marker);
  p = anedya_json_objOpen(p, "data", &marker);
  for (int i = 0; i < req_config->data_count; i++) {
    p = anedya_json_nstr(p, req_config->data[i].key, req_config->data[i].value,
                         strlen(req_config->data[i].value), &marker);
  }
  p = anedya_json_objClose(p, &marker);
  p = anedya_json_objClose(p, &marker);
  p = anedya_json_end(p, &marker);

  // Body is ready now publish it to the MQTT
  char topic[100];
  // printf("Req: %s", txbuffer);
  strcpy(topic, "$anedya/device/");
  strcat(topic, client->config->_device_id_str);
  strcat(topic, "/events/submit/json");
#ifdef ANEDYA_CONNECTION_METHOD_MQTT
  err = anedya_interface_mqtt_publish(client->mqtt_client, topic, strlen(topic),
                                      txbuffer, strlen(txbuffer), 0, 0);
  if (err != ANEDYA_OK) {
    return err;
  }
#endif /* ANEDYA_CONNECTION_METHOD_MQTT */

#ifdef ANEDYA_CONNECTION_METHOD_HTTP
  char resp_buf[ANEDYA_RX_BUFFER_SIZE];
  int resp_len = 0;
  err = _anedya_interface_http_post(client, "/v1/events/submit", txbuffer,
                                    (int)strlen(txbuffer), resp_buf,
                                    ANEDYA_RX_BUFFER_SIZE, &resp_len);
  if (err != ANEDYA_OK) {
    return err;
  }
  _anedya_handle_http_txn_response(client, resp_buf, resp_len, txn);
#endif /* ANEDYA_CONNECTION_METHOD_HTTP */

  return ANEDYA_OK;
}