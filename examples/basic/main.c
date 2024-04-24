#include <stdint.h>
#include <stdio.h>

#include "anedya.h"
#include "anedya_err.h"

void main() {
    anedya_config_t config;
    anedya_client_t client;
    char *conneciton_key = "testconnectionkey";
    anedya_device_id_t devid;

    anedya_err_t err = anedya_parse_device_id("69f984f2-d8e1-4642-9cae-4d615cc25093", devid);
    if (err != ANEDYA_OK) {
        // Check for error
        printf("%s\n", anedya_err_to_name(err));
        return;
    }
    printf("Parsed Device ID: ");
    for (int i = 0; i < 16; i++) {
        printf("%02X ", devid[i]);
    }
    printf("\n");

    err = anedya_init_config(&config, &devid, conneciton_key);
    if (err != ANEDYA_OK) {
        printf("%s\n", anedya_err_to_name(err));
        return;
    }
    err = anedya_set_region(&config, ANEDYA_AP_IN_1);
    if (err != ANEDYA_OK) {
        printf("%s\n", anedya_err_to_name(err));
        return;
    }
    // Set timeout for requests processing. Please note that timeout is in milliseconds.
    err = anedya_set_timeout(&config, 10000);
    if (err != ANEDYA_OK) {
        printf("%s\n", anedya_err_to_name(err));
        return;
    }

    printf("Config initialized\n");
    printf("Connection Key: %s Length: %d\n", config.connection_key, config.connection_key_len);
    printf("Timeout:  %ld\n", config.timeout);
}