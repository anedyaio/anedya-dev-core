#include "anedya_op_submitdata.h"
#include "anedya_operations.h"

#ifdef ANEDYA_CONNECTION_METHOD_HTTP
static anedya_err_t _anedya_http_submit_data(anedya_client_t *client,
                                             anedya_txn_t *txn,
                                             const char *txbuffer) {
#ifdef ANEDYA_ENABLE_STATIC_ALLOCATION
  char resp_buf[ANEDYA_RX_BUFFER_SIZE];
#endif
  int resp_len = 0;

  anedya_err_t err = _anedya_interface_http_post(
      client, "/v1/submitData", txbuffer, (int)strlen(txbuffer), resp_buf,
      ANEDYA_RX_BUFFER_SIZE, &resp_len);

  if (err != ANEDYA_OK) {
    txn->is_complete = true;
    txn->is_success = false;
    txn->_op_err = err;
    _anedya_txn_complete(client, txn);
    return ANEDYA_OK;
  }
  strcpy(txn->_rxbody, resp_buf);
  txn->_rx_len = resp_len + 1;

  if (txn->_rx_len > ANEDYA_RX_BUFFER_SIZE) {
    txn->_op_err = ANEDYA_ERR_RX_BUFFER_OVERFLOW;
    txn->is_complete = true;
    txn->is_success = false;
    _anedya_txn_complete(client, txn);
    return ANEDYA_OK;
  }

  // Call the specific parser for Submit Data operations
  _anedya_device_handle_generic_resp(client, txn);

  // Mark the transaction as complete
  txn->is_complete = true;
  _anedya_txn_complete(client, txn);

  return ANEDYA_OK;
}
#endif /* ANEDYA_CONNECTION_METHOD_HTTP */

/* ────────────────────────────────────────────────────────────────────────────
 * anedya_op_submit_float_req
 * ───────────────────────────────────────────────────────────────────────────
 */
anedya_err_t anedya_op_submit_float_req(anedya_client_t *client,
                                        anedya_txn_t *txn,
                                        const char *variable_identifier,
                                        float value, uint64_t timestamp_ms) {
// First check if client is already connected or not
#ifdef ANEDYA_CONNECTION_METHOD_MQTT
  if (client->is_connected == 0) {
    return ANEDYA_ERR_NOT_CONNECTED;
  }
#endif
  // If it is connected, then create a txn
  txn->_op = ANEDYA_OP_SUBMIT_DATA;
  anedya_err_t err = _anedya_txn_register(client, txn);
  if (err != ANEDYA_OK) {
    return err;
  }

  // Generate the JSON body (identical for both MQTT and HTTP)
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
#ifdef ANEDYA_CONNECTION_METHOD_MQTT
  p = anedya_json_nstr(p, "reqId", slot_number, digitLen, &marker);
#endif
  p = anedya_json_arrOpen(p, "data", &marker);
  p = anedya_json_objOpen(p, NULL, &marker);
  p = anedya_json_str(p, "variable", variable_identifier, &marker);
  p = anedya_json_double(p, "value", value, &marker);
  p = anedya_json_verylong(p, "timestamp", timestamp_ms, &marker);
  p = anedya_json_objClose(p, &marker);
  p = anedya_json_arrClose(p, &marker);
  p = anedya_json_objClose(p, &marker);
  p = anedya_json_end(p, &marker);

#ifdef ANEDYA_CONNECTION_METHOD_MQTT
  // Send via MQTT
  char topic[100];
  strcpy(topic, "$anedya/device/");
  strcat(topic, client->config->_device_id_str);
  strcat(topic, "/submitdata/json");
  err = anedya_interface_mqtt_publish(client->mqtt_client, topic, strlen(topic),
                                      txbuffer, strlen(txbuffer), 0, 0);
  if (err != ANEDYA_OK) {
    return err;
  }
#endif

#ifdef ANEDYA_CONNECTION_METHOD_HTTP
  // Send via HTTP (synchronous)
  err = _anedya_http_submit_data(client, txn, txbuffer);
  if (err != ANEDYA_OK) {
    return err;
  }
#endif

  return ANEDYA_OK;
}

/* ────────────────────────────────────────────────────────────────────────────
 * anedya_op_submit_geo_req
 * ───────────────────────────────────────────────────────────────────────────
 */
anedya_err_t anedya_op_submit_geo_req(anedya_client_t *client,
                                      anedya_txn_t *txn,
                                      const char *variable_identifier,
                                      anedya_geo_data_t *value,
                                      uint64_t timestamp_ms) {
#ifdef ANEDYA_CONNECTION_METHOD_MQTT
  if (client->is_connected == 0) {
    return ANEDYA_ERR_NOT_CONNECTED;
  }
#endif
  txn->_op = ANEDYA_OP_SUBMIT_DATA;
  anedya_err_t err = _anedya_txn_register(client, txn);
  if (err != ANEDYA_OK) {
    return err;
  }

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
#ifdef ANEDYA_CONNECTION_METHOD_MQTT
  p = anedya_json_nstr(p, "reqId", slot_number, digitLen, &marker);
#endif
  p = anedya_json_arrOpen(p, "data", &marker);
  p = anedya_json_objOpen(p, NULL, &marker);
  p = anedya_json_str(p, "variable", variable_identifier, &marker);
  p = anedya_json_objOpen(p, "value", &marker);
  p = anedya_json_double(p, "lat", value->lat, &marker);
  p = anedya_json_double(p, "long", value->lon, &marker);
  p = anedya_json_objClose(p, &marker);
  p = anedya_json_verylong(p, "timestamp", timestamp_ms, &marker);
  p = anedya_json_objClose(p, &marker);
  p = anedya_json_arrClose(p, &marker);
  p = anedya_json_objClose(p, &marker);
  p = anedya_json_end(p, &marker);

#ifdef ANEDYA_CONNECTION_METHOD_MQTT
  char topic[100];
  strcpy(topic, "$anedya/device/");
  strcat(topic, client->config->_device_id_str);
  strcat(topic, "/submitdata/json");
  err = anedya_interface_mqtt_publish(client->mqtt_client, topic, strlen(topic),
                                      txbuffer, strlen(txbuffer), 0, 0);
  if (err != ANEDYA_OK) {
    return err;
  }
#endif

#ifdef ANEDYA_CONNECTION_METHOD_HTTP
  err = _anedya_http_submit_data(client, txn, txbuffer);
  if (err != ANEDYA_OK) {
    return err;
  }
#endif

  return ANEDYA_OK;
}

/* ────────────────────────────────────────────────────────────────────────────
 * anedya_op_submit_status_req
 * ───────────────────────────────────────────────────────────────────────────
 */
anedya_err_t anedya_op_submit_status_req(anedya_client_t *client,
                                         anedya_txn_t *txn,
                                         const char *variable_identifier,
                                         const char *value,
                                         uint64_t timestamp_ms) {
#ifdef ANEDYA_CONNECTION_METHOD_MQTT
  if (client->is_connected == 0) {
    return ANEDYA_ERR_NOT_CONNECTED;
  }
#endif
  txn->_op = ANEDYA_OP_SUBMIT_DATA;
  anedya_err_t err = _anedya_txn_register(client, txn);
  if (err != ANEDYA_OK) {
    return err;
  }

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
#ifdef ANEDYA_CONNECTION_METHOD_MQTT
#ifdef ANEDYA_CONNECTION_METHOD_MQTT
  p = anedya_json_nstr(p, "reqId", slot_number, digitLen, &marker);
#endif
#endif
  p = anedya_json_arrOpen(p, "data", &marker);
  p = anedya_json_objOpen(p, NULL, &marker);
  p = anedya_json_str(p, "variable", variable_identifier, &marker);
  p = anedya_json_str(p, "value", value, &marker);
  p = anedya_json_verylong(p, "timestamp", timestamp_ms, &marker);
  p = anedya_json_objClose(p, &marker);
  p = anedya_json_arrClose(p, &marker);
  p = anedya_json_objClose(p, &marker);
  p = anedya_json_end(p, &marker);

#ifdef ANEDYA_CONNECTION_METHOD_MQTT
  char topic[100];
  strcpy(topic, "$anedya/device/");
  strcat(topic, client->config->_device_id_str);
  strcat(topic, "/submitdata/json");
  err = anedya_interface_mqtt_publish(client->mqtt_client, topic, strlen(topic),
                                      txbuffer, strlen(txbuffer), 0, 0);
  if (err != ANEDYA_OK) {
    return err;
  }
#endif

#ifdef ANEDYA_CONNECTION_METHOD_HTTP
  err = _anedya_http_submit_data(client, txn, txbuffer);
  if (err != ANEDYA_OK) {
    return err;
  }
#endif

  return ANEDYA_OK;
}