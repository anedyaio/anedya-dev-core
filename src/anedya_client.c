#include <stdio.h>
#include "anedya.h"
#include "anedya_certs.h"

anedya_err_t anedya_init_client(anedya_config_t *config, anedya_client_t *client) {
    client->config = config;
    #ifdef ANEDYA_ENABLE_DYNAMIC_ALLOCATION
        client->tx_buffer = anedya_interface_allocate_memory(config->tx_buffer_size);
        if(client->tx_buffer == NULL) {
            return ANEDYA_ERR_NO_MEMORY;
        }
        client->rx_buffer = anedya_interface_allocate_memory(config->rx_buffer_size);
        if(client->rx_buffer == NULL) {
            return ANEDYA_ERR_NO_MEMORY;
        }
        #ifdef ANEDYA_ENABLE_DEVICE_LOGS
            client->log_buffer = anedya_interface_allocate_memory(config->log_buffer_size * config->log_batch_size);
            if(client->log_buffer == NULL) {
                return ANEDYA_ERR_NO_MEMORY;
            }
        #endif
    #endif
    return ANEDYA_OK;
}

anedya_err_t anedya_connect(anedya_client_t *client) {
    anedya_err_t err;
    char url[256];
    char uuid_str[37];
    snprintf(url, 256, "device.%s.anedya.io", client->config->region);
    sprintf(uuid_str, "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
            *client->config->device_id[0], *client->config->device_id[1],
            *client->config->device_id[2], *client->config->device_id[3],
            *client->config->device_id[4], *client->config->device_id[5],
            *client->config->device_id[6], *client->config->device_id[7],
            *client->config->device_id[8], *client->config->device_id[9],
            *client->config->device_id[10], *client->config->device_id[11],
            *client->config->device_id[12], *client->config->device_id[13],
            *client->config->device_id[14], *client->config->device_id[15]);
    printf("UUID: %s\n", uuid_str);
    /*err = anedya_interface_mqtt_connect(
        client,
        url,
        8883,
        uuid_str,
        client->config->connection_key,
        client->config->connection_key_len,
        uuid_str,
        anedya_tls_root_ca, anedya_tls_root_ca_len);
    if (err != ANEDYA_OK) {
        return err;
    }*/
    return ANEDYA_OK;
}