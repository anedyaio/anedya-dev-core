#pragma once

#ifndef _ANEDYA_REQ_H_
#define _ANEDYA_REQ_H_

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "anedya_sdk_config.h"
#include "anedya_commons.h"

#ifdef __cplusplus
extern "C"
{
#endif

        typedef struct
        {
#ifdef ANEDYA_ENABLE_STATIC_ALLOCATION
                char key[50];
                char value[50];
#endif
#ifdef ANEDYA_ENABLE_DYNAMIC_ALLOCATION
                char *key;
                char *value;
#endif
                size_t key_len;
                size_t value_len;
        } anedya_asset_metadata_t;

        typedef struct
        {
                anedya_uuid_t asset_id;
#ifdef ANEDYA_ENABLE_STATIC_ALLOCATION
                char asset_identifier[50];
                char asset_version[50];
                char asset_checksum[255];
                char asset_url[1000];
                char asset_signature[255];
#endif
#ifdef ANEDYA_ENABLE_DYNAMIC_ALLOCATION
                char *asset_identifier;
                char *asset_version;
                char *asset_checksum;
                char *asset_url;
#endif
                anedya_asset_metadata_t *asset_metadata;
                size_t asset_metadata_len;
                size_t asset_identifier_len;
                size_t asset_version_len;
                size_t asset_checksum_len;
                size_t asset_url_len;
                bool asset_signed;
                size_t asset_signature_len;
                size_t asset_size;
        } anedya_asset_t;

        typedef struct
        {
                anedya_device_id_str_t devId;
                char *binding_secret;
                size_t binding_secret_len;
        } anedya_req_bind_device_t;

        /**
         * @brief Parse a UUID string into a binary format.
         *
         * This function parses a UUID string into the `anedya_uuid_t` binary format. The input string must follow
         * the standard UUID format (e.g., "123e4567-e89b-12d3-a456-426614174000").
         *
         * @param[in] in Pointer to the null-terminated string containing the UUID to parse.
         * @param[out] uuid Parsed output UUID in binary format as an `anedya_uuid_t`.
         *
         * @retval - `ANEDYA_OK` if the UUID is parsed successfully.
         * @retval - `ANEDYA_ERR_INVALID_UUID` if the input string is not in a valid UUID format.
         *
         * @note Ensure the input string follows the correct UUID format for successful parsing.
         */
        anedya_err_t _anedya_uuid_parse(const char *in, anedya_uuid_t uuid);

        /**
         * @brief Convert a binary UUID to a string format.
         *
         * This function converts a binary UUID (`anedya_uuid_t`) to its string representation in the
         * standard UUID format (e.g., "123e4567-e89b-12d3-a456-426614174000").
         *
         * @param[in] uuid Binary UUID input as an `anedya_uuid_t`.
         * @param[out] out Pointer to the output buffer for the UUID string. Must be at least 37 bytes long
         *                 to accommodate the null-terminated string format.
         *
         * @note Ensure that the output buffer is adequately sized to store the resulting UUID string.
         */
        void _anedya_uuid_marshal(const anedya_uuid_t uuid, char *out);

#ifdef __cplusplus
}
#endif

#endif