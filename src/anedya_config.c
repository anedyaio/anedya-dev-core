#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "anedya.h"
#include "anedya_err.h"
#include "anedya_config.h"

#define parsehex(x) (('0' <= x && x <= '9') ? \
    (x - '0') : \
    (('a' <= x && x <= 'f') ? \
        (10 + (x - 'a')) : \
        (('A' <= x && x <= 'F') ? (10 + (x - 'A')) : (-1))))



anedya_err_t anedya_parse_device_id(const char deviceID[37], anedya_device_id_t devID) {
    static const int8_t si[16] = {0,2,4,6,9,11,14,16,19,21,24,26,28,30,32,34};
    if(strnlen(deviceID, 37) != 36) {
        return ANEDYA_INVALID_DEVICE_ID;
    }
    for(int i = 0; i < 36; i++) {
        if(i == 8 || i == 13 || i == 18 || i == 23) {
            if(deviceID[i] != '-') {
                return ANEDYA_INVALID_DEVICE_ID;
            }
        } else {
            if(parsehex(deviceID[i]) == -1) {
                return ANEDYA_INVALID_DEVICE_ID;
            }
        }
    }
    for (int i = 0; i < 16; i++) {
		int8_t hi = (int8_t)parsehex(deviceID[si[i] + 0]);
		int8_t lo = (int8_t)parsehex(deviceID[si[i] + 1]);
		devID[i] = ((hi << 4) | lo);
	}
    return ANEDYA_OK;
}


anedya_err_t anedya_init_config(anedya_config_t *config, anedya_device_id_t *devId, const char *connection_key) {
    config->connection_key = connection_key;
    config->connection_key_len = strlen(connection_key);
    config->device_id = devId;
    return ANEDYA_OK;
}

anedya_err_t anedya_set_region(anedya_config_t *config, const char *region) {
    if(strcmp(region, ANEDYA_AP_IN_1) == 0) {
        config->region = ANEDYA_AP_IN_1;
    } else {
        return ANEDYA_INVALID_REGION;
    }
    return ANEDYA_OK;
}

anedya_err_t anedya_set_timeout(anedya_config_t *config, size_t timeout) {
    if(timeout < 0) {
        return ANEDYA_ERR;
    }
    config->timeout = timeout;
}