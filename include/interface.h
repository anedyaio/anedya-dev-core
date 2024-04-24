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

#include "config.h"
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef ANEDYA_ENABLE_DEBUG_OUTPUT
/** @brief: Defines an interface for providing output to serial console or any other terminal on the hardware*/
void anedya_interface_std_out(const char* str);
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

#ifdef __cplusplus
}
#endif