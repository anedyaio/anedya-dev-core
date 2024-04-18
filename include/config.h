/*
* This file is part of Anedya Core SDK
* (c) 2024, Anedya Systems Private Limited
*/

#pragma once

// Include error definitions, omit these definitions reduces binary size by a small margin
#define ANEDYA_INCLUDE_ERR_NAMES

// Define different buffer sizes(in bytes) to preallocate memeory to carry out different transactions
// Please provide sufficient buffer according to your requirement.
// Lower buffer will not be able to include multiple datapoint submission in single request
// Higher buffer will be able to include multiple datapoint submission in single request but will occupy higher SRAM
// Default buffer size is 1024 bytes
#define ANEDYA_TX_BUFFER 1024
#define ANEDYA_RX_BUFFER 1024