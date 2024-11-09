#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Log a debug message for Anedya.
 *
 * This function outputs a debug message to the serial console.
 *
 * @param[in] log Pointer to a null-terminated string containing the debug message to log.
 *
 */
void anedya_debug_log(const char *log);

#ifdef __cplusplus
}
#endif