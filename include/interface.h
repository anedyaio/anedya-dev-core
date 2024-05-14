/*
* This file is part of Anedya Core SDK
* (c) 2024, Anedya Systems Private Limited
*/

#pragma once

/*
Interface file defines all the interfaces which are required by the Anedya Core Functions.
This core library makes no assumptions about the underlying architecture and thus it is assumed that no standard C libraries are 
available at runtime. This library requires interfaces to be implemented by the platform layer.

This library uses static memory allocation in place of dynamic memory allocation to make it compaitible with almost all hardware platforms

See config.h for tuning operations of this library.
*/

#include <stddef.h>
#include <stdint.h>
#include "anedya_err.h"
#include "config.h"
#include "anedya_commons.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef ANEDYA_ENABLE_DEBUG_OUTPUT
/** @brief: Defines an interface for providing output to serial console or any other terminal on the hardware*/
void anedya_interface_std_out(const char* str);
#endif

#ifdef ANEDYA_ENABLE_DYNAMIC_ALLOCATION
void* anedya_interface_allocate_memory(size_t size);
#endif

/** @brief: An interface used internally to sleep the code for milliseconds provided.*/
void anedya_interface_sleep_ms(size_t milli);

/** @brief: An interface used internally to get the current system time in milliseconds.*/
uint64_t anedya_interface_get_time_ms();

/** @brief: An interface used internally to set the current system time in milliseconds.*/
void anedya_interface_set_time_ms(uint64_t time);

//==================================================================================
// CONNECTION MANAGEMENT
//==================================================================================

/*

MQTT Connection Management Interface:

There are two type of interface methods:
    - Outbound methods - Which needs to be implemented by the platform layer
    - Inbound methods - Which are implemented by the core library and called by the platform layer on certain event like receiving message on MQTT

The core library doesn't implement MQTT stack. It needs to be part of the platform layer and it allows developers to use any MQTT stack
supported by the hardware ecosystem.
*/

#ifdef ANEDYA_CONNECTION_METHOD_MQTT
    /** @brief: This method gets called whenever a connection to the broker needs to be made.
     * This method should implement following behaviour:
     * - If connection fails it should immediately return error back to the core instead of retyring the connection as retries will be managed by the core library
     * - For error codes related to MQTT please have a look at the anedya_err.h
     * - If connection gets established, the interface should return ANEDYA_OK error code.
    */
    anedya_err_t anedya_interface_mqtt_connect(anedya_client_t *client, char *broker, int port, char *username, const char *password, int passwordlen, char *client_id, const char *der_cert, const int der_cert_len);

    /** @brief: Function to terminate the MQTT connection
    */
    anedya_err_t anedya_interface_mqtt_disconnect(anedya_client_t *client);

    /** @brief: This method is called to check the current status of the connection.
     *  This method should return 0 if connection is establisehd and any other value if connection is not active. 
    */
    size_t anedya_interface_mqtt_status(anedya_client_t *client);

    /** @brief: Subscribe a topic specified by the library
     * - Returns error code as described in anedya_err.h
     * - If the topic is subscribed successfully, it should return ANEDYA_OK
    */
    anedya_err_t anedya_interface_mqtt_subscribe(anedya_client_t *client,char *topic, int topilc_len, int qos);

    anedya_err_t anedya_interface_mqtt_unsubscribe(anedya_client_t *client,char *topic, int topic_len);

    anedya_err_t anedya_interface_mqtt_publish(anedya_client_t *client,char *topic, int topic_len, char *payload, int payload_len, int qos);
#endif



#ifdef __cplusplus
}
#endif