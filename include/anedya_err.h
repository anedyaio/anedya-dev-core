/*
* This file is part of Anedya Core SDK
* (c) 2024, Anedya Systems Private Limited
*/

#pragma once

#ifdef ___cpluplus
extern "C" {
#endif

typedef int anedya_err_t;

const char *anedya_err_to_name(anedya_err_t code);

#define ANEDYA_OK 0
#define ANEDYA_ERR -1

/* Anedya Error Definitions and Constants */
#define ANEDYA_INVALID_REGION 1
#define ANEDYA_INVALID_DEVICE_ID 2



#ifdef ___cpluplus
}
#endif
