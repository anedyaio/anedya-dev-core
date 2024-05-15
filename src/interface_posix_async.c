/**
 * This interface provides an example interface implementation for posix APIs
 * this interface uses Paho MQTT C library for MQTT connection. This interface
 * assumes async operation in MQTT connection which is common for most linux operated devices.
 * 
 * This interface will use Paho MQTT in async mode and connection management will be running in background in a
 * separate thread.
 * 
 * The interface does not implement thread safety.
*/

#include "config.h"
#include "interface.h"
#include <stddef.h>
#include <time.h>

// Define memory allocation functions

#ifdef ANEDYA_ENABLE_DYNAMIC_ALLOCATION
void* anedya_interface_malloc(size_t size) {
    // In posix we simply return the malloc. When using dynamic memory allocation on other hardware platforms,
    // implement platform specific memory allocation here.
    return malloc(size);
}

void anedya_interface_free(void *ptr) {
    // In posix we simply return the free. When using dynamic memory allocation on other hardware platforms,
    // implement platform specific memory allocation here.
    free(ptr);
}

#endif

void anedya_interface_sleep_micro(size_t micro) {
    usleep(micro);
}

uint64_t anedya_interface_get_time_ms() {
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

void anedya_interface_set_time_ms(uint64_t time) {

}

#ifdef ANEDYA_CONNECTION_METHOD_MQTT
anedya_err_t anedya_interface_mqtt_connect(anedya_mqtt_client_handle_t *client, char *broker, int port, char *username, const char *password, int passwordlen, char *client_id, const char *der_cert, const int der_cert_len) {

    return ANEDYA_OK;    
}


#endif