#include <stdint.h>
#include <stdio.h>

#include "anedya.h"

void main() {
    anedya_config_t config;
    anedya_client_t client;
    char *conneciton_key = "markandpathak";
    anedya_device_id_t devid;

    printf("Hello, World!\n");

    anedya_err_t err = anedya_parse_device_id("69f984f2-d8e1-4642-9cae-4d615cc25093", devid);
    if (err != ANEDYA_OK) {
        printf("Error parsing device id\n");
        return;
    }

    for (int i = 0; i < 16; i++) {
        printf("%02X ", devid[i]);
    }
    printf("\n");

    err = anedya_init_config(&config, &devid, conneciton_key);
    if (err != ANEDYA_OK) {
        printf("Error initializing config\n");
        return;
    }

    printf("Config initialized\n");
    printf("Config: %s Length: %d\n", config.connection_key, config.connection_key_len);
}