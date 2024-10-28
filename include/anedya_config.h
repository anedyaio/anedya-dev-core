#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#define UUID_STRING_LEN 37

        typedef struct
        {
                size_t timeout;
                const char *region;
                const char *connection_key;
                unsigned int connection_key_len;
                anedya_device_id_t device_id;
                anedya_device_id_str_t _device_id_str;
// MQTT Callbacks
#ifdef ANEDYA_CONNECTION_METHOD_MQTT
                anedya_on_connect_cb_t on_connect;
                anedya_context_t on_connect_ctx;
                anedya_on_disconnect_cb_t on_disconnect;
                anedya_context_t on_disconnect_ctx;
                anedya_event_handler_t event_handler;
                anedya_context_t event_handler_ctx;
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

        /**
         * @brief Parses a device ID string into a binary array format.
         *
         * This function validates and parses a 36-character device ID string, formatted as a UUID
         * with hyphens at positions 8, 13, 18, and 23 (e.g., "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx").
         * The function converts the UUID string into a 16-byte binary format and stores it in the
         * provided `devID` array of type `anedya_device_id_t`.
         *
         * @param[in] deviceID The input device ID string in UUID format (36 characters including hyphens, plus the null-terminator).
         *                     It must contain 32 hexadecimal digits separated by hyphens.
         * @param[out] devID   An `anedya_device_id_t` array (16 bytes) that receives the parsed binary device ID.
         *
         * @retval `ANEDYA_OK` on successful parsing.
         * @retval `ANEDYA_ERR_INVALID_DEVICE_ID` if the input `deviceID` is incorrectly formatted.
         *
         * @note This function requires the device ID string to be exactly 36 characters in length, including
         *       hyphens. Each hexadecimal character is validated, and invalid characters result in an error.
         *
         * @warning The output array `devID` must be large enough to hold 16 bytes.
         */
        anedya_err_t anedya_parse_device_id(const char deviceID[37], anedya_device_id_t devID);

        /**
         * @brief Initializes the configuration structure for an Anedya client.
         *
         * This function sets up the `anedya_config_t` structure by assigning the connection key,
         * connection key length, and device ID. It also converts the binary device ID (`devId`)
         * into a UUID string format and stores it in the `_device_id_str` field of the configuration.
         * Additionally, all event handlers are initialized to `NULL`.
         *
         * @param[out] config            Pointer to the `anedya_config_t` structure to initialize.
         * @param[in]  devId             The binary device ID in the form of `anedya_device_id_t`.
         * @param[in]  connection_key    Pointer to a string containing the connection key.
         * @param[in]  connection_key_len Length of the connection key string.
         *
         * @return
         *      - `ANEDYA_OK` if initialization is successful.
         *
         * @note This function sets the event handler, on_connect, and on_disconnect callbacks to `NULL`.
         *       Ensure to set these callbacks if needed after initialization.
         *
         * @warning The `config` pointer must point to valid memory, as this function will write data
         *          directly to the fields of `config`.
         */
        anedya_err_t anedya_config_init(anedya_config_t *config, anedya_device_id_t devId, const char *connection_key, size_t connection_key_len);

        anedya_err_t anedya_config_set_region(anedya_config_t *config, const char *region);

        /** @brief: TODO */
        anedya_err_t anedya_config_set_timeout(anedya_config_t *config, size_t timeout_sec);

#ifdef ANEDYA_CONNECTION_METHOD_MQTT
        /** @brief: TODO */
        anedya_err_t anedya_config_set_connect_cb(anedya_config_t *config, anedya_on_connect_cb_t on_connect, anedya_context_t ctx);
        /** @brief: TODO */
        anedya_err_t anedya_config_set_disconnect_cb(anedya_config_t *config, anedya_on_disconnect_cb_t on_disconnect, anedya_context_t ctx);
        /** @brief: TODO */
        anedya_err_t anedya_config_register_event_handler(anedya_config_t *config, anedya_event_handler_t event_handler, anedya_context_t ctx);
#endif

#ifdef __cplusplus
}
#endif