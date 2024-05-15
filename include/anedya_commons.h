#pragma once

#include <stdlib.h>
#include "config.h"
#include "anedya_err.h"

#ifdef __cplusplus
extern "C" {
#endif

// Region Codes
#define ANEDYA_REGION_AP_IN_1 "ap-in-1"

#define ANEDYA_URL_PREFIX "device."
#define ANEDYA_URL_SUFFIX ".anedya.com"

// Common datatypes

typedef unsigned char anedya_device_id_t[16];
typedef unsigned int anedya_client_descriptor_t;

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
    const char *region;
    const char *connection_key;
    unsigned int connection_key_len;
    anedya_device_id_t device_id;
    // MQTT Callbacks
    #ifdef ANEDYA_CONNECTION_METHOD_MQTT
        anedya_on_connect_cb_t on_connect;
        anedya_on_disconnect_cb_t on_disconnect;
        anedya_on_command_cb_t on_command;
    #endif
    // In case of dynamic memory allocation, set the initial sizes of buffers
    #ifdef ANEDYA_ENABLE_DYNAMIC_ALLOCATION
        size_t tx_buffer_size;
        size_t rx_buffer_size;
        #ifdef ANEDYA_ENABLE_DEVICE_LOGS
            size_t log_buffer_size;
            size_t log_batch_size;
        #endif
        size_t datapoints_batch_size;
    #endif
} anedya_config_t;

typedef struct {
    anedya_config_t *config;
    #ifdef ANEDYA_ENABLE_STATIC_ALLOCATION
        char tx_buffer[ANEDYA_TX_BUFFER];
        char rx_buffer[ANEDYA_RX_BUFFER];
        char log_buffer[ANEDYA_LOG_BUFFER * ANEDYA_MAX_LOG_BATCH];
    #endif
    #ifdef ANEDYA_ENABLE_DYNAMIC_ALLOCATION
        char *tx_buffer;
        char *rx_buffer;
        #ifdef ANEDYA_ENABLE_DEVICE_LOGS
            char *log_buffer;
        #endif
    #endif
    #ifdef ANEDYA_CONNECTION_METHOD_MQTT
        anedya_mqtt_client_handle_t mqtt_client;
    #endif
} anedya_client_t;

#ifdef __cplusplus
}
#endif