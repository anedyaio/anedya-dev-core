#pragma once

#include "anedya_commons.h"
#include "anedya_device_logs.h"
#include "anedya_operations.h"
#include "config.h"
#include "interface.h"
#include "anedya_err.h"
// Anedya Core Library
#ifdef __cplusplus
extern "C" {
#endif

anedya_err_t anedya_parse_device_id(const char deviceID[37], anedya_device_id_t devID);

// Config management APIs
/** @brief: Initialize a config with default values*/
anedya_err_t anedya_init_config(anedya_config_t *config, anedya_device_id_t *devId, const char *connection_key);

anedya_err_t anedya_set_region(anedya_config_t *config, const char *region);

anedya_err_t anedya_set_timeout(anedya_config_t *config, size_t timeout);

#ifdef ANEDYA_CONNECTION_METHOD_MQTT

anedya_err_t anedya_set_connect_cb(anedya_config_t *config, anedya_on_connect_cb_t on_connect);

anedya_err_t anedya_set_disconnect_cb(anedya_config_t *config, anedya_on_disconnect_cb_t on_disconnect);

anedya_err_t anedya_set_command_cb(anedya_config_t *config, anedya_on_command_cb_t on_command);

#endif

// Client APIs

anedya_err_t anedya_init_client(anedya_config_t *config, anedya_client_t *client);

#ifdef ANEDYA_CONNECTION_METHOD_MQTT
/** @brief: Initiate connection with the Anedya platform.*/
anedya_err_t anedya_connect(anedya_client_t *client);

anedya_err_t anedya_disconnect(anedya_client_t *client);
#endif

#ifdef __cplusplus
}
#endif