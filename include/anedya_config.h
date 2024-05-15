#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define UUID_STRING_LEN 37

anedya_err_t anedya_parse_device_id(const char deviceID[37], anedya_device_id_t devID);

anedya_err_t anedya_init_config(anedya_config_t *config, anedya_device_id_t devId, const char *connection_key);

anedya_err_t anedya_set_region(anedya_config_t *config, const char *region);

anedya_err_t anedya_set_timeout(anedya_config_t *config, size_t timeout);

#ifdef __cplusplus
}
#endif