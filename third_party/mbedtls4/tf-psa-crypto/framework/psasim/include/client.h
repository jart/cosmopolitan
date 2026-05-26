/* PSA Firmware Framework client header for psasim. */

/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#ifndef __PSA_CLIENT_H__
#define __PSA_CLIENT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

#include "psa/crypto.h"

#include "error_ext.h"
/*********************** PSA Client Macros and Types *************************/

#define PSA_FRAMEWORK_VERSION  (0x0100)

#define PSA_VERSION_NONE       (0)

/* PSA response types */
#define PSA_CONNECTION_REFUSED PSA_ERROR_CONNECTION_REFUSED
#define PSA_CONNECTION_BUSY    PSA_ERROR_CONNECTION_BUSY
#define PSA_DROP_CONNECTION    PSA_ERROR_PROGRAMMER_ERROR

/* PSA message handles */
#define PSA_NULL_HANDLE        ((psa_handle_t) 0)

#define PSA_HANDLE_IS_VALID(handle) ((psa_handle_t) (handle) > 0)
#define PSA_HANDLE_TO_ERROR(handle) ((psa_status_t) (handle))

/**
 * A read-only input memory region provided to an RoT Service.
 */
typedef struct psa_invec {
    const void *base;
    size_t len;
} psa_invec;

/**
 * A writable output memory region provided to an RoT Service.
 */
typedef struct psa_outvec {
    void *base;
    size_t len;
} psa_outvec;

/*************************** PSA Client API **********************************/

uint32_t psa_framework_version(void);

uint32_t psa_version(uint32_t sid);

psa_handle_t psa_connect(uint32_t sid, uint32_t version);

psa_status_t psa_call(psa_handle_t handle,
                      int32_t type,
                      const psa_invec *in_vec,
                      size_t in_len,
                      psa_outvec *out_vec,
                      size_t out_len);

void psa_close(psa_handle_t handle);

#ifdef __cplusplus
}
#endif

#endif /* __PSA_CLIENT_H__ */
