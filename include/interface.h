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

#ifdef __cplusplus
extern "C" {
#endif



#ifdef __cplusplus
}
#endif