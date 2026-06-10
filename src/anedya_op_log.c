#include "anedya_operations.h"

anedya_err_t anedya_op_submit_log(anedya_client_t *client, anedya_txn_t *txn,
                                  char *log, unsigned int log_len,
                                  unsigned long long timestamp_ms) {
// First check if client is already connected or not
#ifdef ANEDYA_CONNECTION_METHOD_MQTT
  if (client->is_connected == 0) {
    return ANEDYA_ERR_NOT_CONNECTED;
  }
#endif
  // If it is connected, then create a txn
  txn->_op = ANEDYA_OP_SUBMIT_LOG;
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
  p = anedya_json_nstr(p, "reqId", slot_number, digitLen, &marker);
  p = anedya_json_arrOpen(p, "data", &marker);
  p = anedya_json_objOpen(p, NULL, &marker);
  p = anedya_json_nstr(p, "log", log, log_len, &marker);
  p = anedya_json_verylong(p, "timestamp", timestamp_ms, &marker);
  p = anedya_json_objClose(p, &marker);
  p = anedya_json_arrClose(p, &marker);
  p = anedya_json_objClose(p, &marker);
  p = anedya_json_end(p, &marker);

  // Body is ready now publish it to the MQTT
  char topic[100];
  // printf("Req: %s", txbuffer);
  strcpy(topic, "$anedya/device/");
  strcat(topic, client->config->_device_id_str);
  strcat(topic, "/logs/submitLogs/json");
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
  err = _anedya_interface_http_post(client, "/v1/logs/submitLogs", txbuffer,
                                    (int)strlen(txbuffer), resp_buf,
                                    ANEDYA_RX_BUFFER_SIZE, &resp_len);
  if (err != ANEDYA_OK) {
    return err;
  }
  _anedya_handle_http_txn_response(client, resp_buf, resp_len, txn);
#endif /* ANEDYA_CONNECTION_METHOD_HTTP */

  return ANEDYA_OK;
}