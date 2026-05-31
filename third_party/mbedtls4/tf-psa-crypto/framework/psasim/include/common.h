/* Common definitions used for clients and services */

/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdint.h>
#include <stddef.h>

/* Increasing this might break on some platforms */
#define MAX_FRAGMENT_SIZE 200

#define CONNECT_REQUEST 1
#define CALL_REQUEST 2
#define CLOSE_REQUEST 3
#define VERSION_REQUEST 4
#define READ_REQUEST    5
#define READ_RESPONSE   6
#define WRITE_REQUEST   7
#define WRITE_RESPONSE  8
#define SKIP_REQUEST    9
#define PSA_REPLY       10

#define NON_SECURE (1 << 30)

typedef int32_t psa_handle_t;

#define PSA_MAX_IOVEC (4u)

#define PSA_IPC_CALL (0)

struct message_text {
    int qid;
    int32_t psa_type;
    char buf[MAX_FRAGMENT_SIZE];
};

struct message {
    long message_type;
    struct message_text message_text;
};

typedef struct vector_sizes {
    size_t invec_sizes[PSA_MAX_IOVEC];
    size_t outvec_sizes[PSA_MAX_IOVEC];
} vector_sizes_t;

#endif /* _COMMON_H_ */
