#pragma once

#ifndef _ANEDYA_TXN_H_
#define _ANEDYA_TXN_H_

#include <stdint.h>
#include <stdbool.h>
#include "anedya_err.h"
#include "anedya_client.h"
#include "anedya_sdk_config.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef uint8_t anedya_txn_desc_t;
    typedef size_t anedya_op_t;
    typedef size_t anedya_op_err_t;
    typedef void *anedya_response_t;
    typedef void (*anedya_txn_cb_t)(anedya_txn_t *txn, anedya_context_t ctx);

    struct anedya_txn_t
    {
        anedya_txn_desc_t desc;
#ifdef ANEDYA_ENABLE_DYNAMIC_ALLOCATION
        char *_txbody;
        char *_rxbody;
#endif
#ifdef ANEDYA_ENABLE_STATIC_ALLOCATION
        char _rxbody[ANEDYA_RX_BUFFER_SIZE];
#endif
        size_t _rx_len;
        bool is_complete;
        bool is_success;
        anedya_context_t ctx;
        anedya_txn_cb_t callback;
        anedya_op_t _op;
        anedya_err_t _op_err;
        anedya_response_t response;
    };
    /** @defgroup anedytxn Anedya Transaction 
     * @{  */

    /**
     * @brief Register a callback function for a transaction.
     *
     * This function associates a user-defined callback function and context with a specific transaction.
     * The callback will be triggered when the transaction completes, allowing for custom handling of
     * transaction results.
     *
     * @param[in,out] txn Pointer to the `anedya_txn_t` structure representing the transaction.
     * @param[in] callback The callback function to be registered for the transaction. The callback will
     *                     be executed upon completion of the transaction.
     * @param[in] ctx User-defined context to be passed to the callback function. This context can be
     *                used to store additional information or parameters needed within the callback.
     *
     * @retval None
     *
     * @note Ensure that the callback and context are valid before assigning them to the transaction.
     */
    void anedya_txn_register_callback(anedya_txn_t *txn, anedya_txn_cb_t callback, anedya_context_t ctx);

    anedya_err_t _anedya_txn_register(anedya_client_t *client, anedya_txn_t *txn);
    anedya_err_t _anedya_txn_complete(anedya_client_t *client, anedya_txn_t *txn);
    anedya_txn_desc_t _anedya_txn_get_desc(anedya_txn_t *txn);

    /** @} */

#ifdef __cplusplus
}
#endif

#endif /* _ANEDYA_TXN_H_ */
