#pragma once

#include <stdlib.h>
#include "config.h"
#include "anedya_err.h"

#ifdef __cplusplus
extern "C" {
#endif

// Region Codes
#define ANEDYA_REGION_AP_IN_1 "ap-in-1"

// Common datatypes

typedef unsigned char anedya_device_id_t[16];

typedef struct {

} anedya_command_t;

#ifdef ANEDYA_CONNECTION_METHOD_MQTT
    /** @brief: On connect callback, which is called when connection is established with the MQTT Server */
    typedef void (*anedya_on_connect_cb_t)(void);
    /** @brief: On connect callback, which is called when client is disconnected with the MQTT Server. It can be intentional or due to some other error */
    typedef void (*anedya_on_disconnect_cb_t)(void);
    /** @brief: On command callback, which is called when a command is received from the MQTT Server */
    typedef void (*anedya_on_command_cb_t) (anedya_command_t *command);
#endif

typedef struct {
    size_t timeout;
    char *region;
    const char *connection_key;
    unsigned int connection_key_len;
    anedya_device_id_t *device_id;
    #ifdef ANEDYA_CONNECTION_METHOD_MQTT
        anedya_on_connect_cb_t on_connect;
        anedya_on_disconnect_cb_t on_disconnect;
        anedya_on_command_cb_t on_command;
    #endif
} anedya_config_t;

typedef struct {
    anedya_config_t *config;
} anedya_client_t;



#ifdef __cplusplus
}
#endif