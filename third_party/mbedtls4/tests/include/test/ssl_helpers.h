/** \file ssl_helpers.h
 *
 * \brief This file contains helper functions to set up a TLS connection.
 */

/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#ifndef SSL_HELPERS_H
#define SSL_HELPERS_H

#include "mbedtls/build_info.h"

#include <string.h>

#include <test/helpers.h>
#include <test/macros.h>
#include <test/random.h>
#include <test/psa_crypto_helpers.h>

#if defined(MBEDTLS_SSL_TLS_C)
#include <ssl_misc.h>
#include <mbedtls/timing.h>
#include <mbedtls/debug.h>

#include "test/certs.h"

#if defined(MBEDTLS_SSL_CACHE_C)
#include "mbedtls/ssl_cache.h"
#endif

#define PSA_TO_MBEDTLS_ERR(status) PSA_TO_MBEDTLS_ERR_LIST(status, \
                                                           psa_to_ssl_errors, \
                                                           psa_generic_status_to_mbedtls)

#if defined(MBEDTLS_SSL_PROTO_TLS1_3)
#if defined(PSA_WANT_KEY_TYPE_AES)
#if defined(PSA_WANT_ALG_GCM)
#if defined(PSA_WANT_ALG_SHA_384)
#define MBEDTLS_TEST_HAS_TLS1_3_AES_256_GCM_SHA384
#endif
#if defined(PSA_WANT_ALG_SHA_256)
#define MBEDTLS_TEST_HAS_TLS1_3_AES_128_GCM_SHA256
#endif
#endif /* PSA_WANT_ALG_GCM */
#if defined(PSA_WANT_ALG_CCM) && defined(PSA_WANT_ALG_SHA_256)
#define MBEDTLS_TEST_HAS_TLS1_3_AES_128_CCM_SHA256
#define MBEDTLS_TEST_HAS_TLS1_3_AES_128_CCM_8_SHA256
#endif
#endif /* PSA_WANT_KEY_TYPE_AES */
#if defined(PSA_WANT_ALG_CHACHA20_POLY1305) && defined(PSA_WANT_ALG_SHA_256)
#define MBEDTLS_TEST_HAS_TLS1_3_CHACHA20_POLY1305_SHA256
#endif

#if defined(MBEDTLS_TEST_HAS_TLS1_3_AES_256_GCM_SHA384) || \
    defined(MBEDTLS_TEST_HAS_TLS1_3_AES_128_GCM_SHA256) || \
    defined(MBEDTLS_TEST_HAS_TLS1_3_AES_128_CCM_SHA256) || \
    defined(MBEDTLS_TEST_HAS_TLS1_3_AES_128_CCM_8_SHA256) || \
    defined(MBEDTLS_TEST_HAS_TLS1_3_CHACHA20_POLY1305_SHA256)
#define MBEDTLS_TEST_AT_LEAST_ONE_TLS1_3_CIPHERSUITE
#endif

#endif /* MBEDTLS_SSL_PROTO_TLS1_3 */

#if defined(MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED)
#define MBEDTLS_CAN_HANDLE_RSA_TEST_KEY
#endif

#if defined(MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED) && \
    defined(PSA_WANT_ECC_SECP_R1_384) && \
    defined(PSA_WANT_ALG_SHA_384)
#define MBEDTLS_CAN_HANDLE_ECDSA_TEST_KEY
#endif

#if defined(MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED) && \
    defined(PSA_WANT_ECC_SECP_R1_256) && \
    defined(PSA_WANT_ALG_SHA_256)
#define MBEDTLS_CAN_HANDLE_ECDSA_CLIENT_TEST_KEY
#endif

#if defined(PSA_WANT_ECC_MONTGOMERY_255)        || \
    defined(PSA_WANT_ECC_SECP_R1_256)           || \
    defined(PSA_WANT_ECC_SECP_R1_384)           || \
    defined(PSA_WANT_ECC_MONTGOMERY_448)        || \
    defined(PSA_WANT_ECC_SECP_R1_521)           || \
    defined(PSA_WANT_ECC_BRAINPOOL_P_R1_256)    || \
    defined(PSA_WANT_ECC_BRAINPOOL_P_R1_384)    || \
    defined(PSA_WANT_ECC_BRAINPOOL_P_R1_512)
#define MBEDTLS_TEST_HAS_DEFAULT_EC_GROUP
#endif

#if defined(PSA_WANT_ALG_GCM) || \
    defined(PSA_WANT_ALG_CCM) || \
    defined(PSA_WANT_ALG_CHACHA20_POLY1305)
#define MBEDTLS_TEST_HAS_AEAD_ALG
#endif

/*
 * To use the test keys we need PSA_WANT_ALG_SHA_256. Some test cases need an additional hash that
 * is configured by default (see mbedtls_ssl_config_defaults()), but it doesn't matter which one.
 */
#if defined(PSA_WANT_ALG_SHA_512) || \
    defined(PSA_WANT_ALG_SHA_384)
#define MBEDTLS_TEST_HAS_ADDITIONAL_HASH
#endif

enum {
#define MBEDTLS_SSL_TLS1_3_LABEL(name, string)          \
    tls13_label_ ## name,
    MBEDTLS_SSL_TLS1_3_LABEL_LIST
#undef MBEDTLS_SSL_TLS1_3_LABEL
};

#if defined(MBEDTLS_SSL_ALPN)
#define MBEDTLS_TEST_MAX_ALPN_LIST_SIZE 10
#endif

typedef struct mbedtls_test_ssl_log_pattern {
    const char *pattern;
    size_t counter;
} mbedtls_test_ssl_log_pattern;

typedef struct mbedtls_test_handshake_test_options {
    const char *cipher;
    uint16_t *group_list;
    mbedtls_ssl_protocol_version client_min_version;
    mbedtls_ssl_protocol_version client_max_version;
    mbedtls_ssl_protocol_version server_min_version;
    mbedtls_ssl_protocol_version server_max_version;
    mbedtls_ssl_protocol_version expected_negotiated_version;
    int expected_handshake_result;
    int expected_ciphersuite;
    int pk_alg;
    int opaque_alg;
    int opaque_alg2;
    int opaque_usage;
    data_t *psk_str;
    int dtls;
    int srv_auth_mode;
    int serialize;
    int mfl;
    int cli_msg_len;
    int srv_msg_len;
    int expected_cli_fragments;
    int expected_srv_fragments;
    int renegotiate;
    int legacy_renegotiation;
    void *srv_log_obj;
    void *cli_log_obj;
    void (*srv_log_fun)(void *, int, const char *, int, const char *);
    void (*cli_log_fun)(void *, int, const char *, int, const char *);
    int resize_buffers;
    int early_data;
    int max_early_data_size;
#if defined(MBEDTLS_SSL_CACHE_C)
    mbedtls_ssl_cache_context *cache;
#endif
#if defined(MBEDTLS_SSL_ALPN)
    const char *alpn_list[MBEDTLS_TEST_MAX_ALPN_LIST_SIZE];
#endif
} mbedtls_test_handshake_test_options;

/*
 * Buffer structure for custom I/O callbacks.
 */
typedef struct mbedtls_test_ssl_buffer {
    size_t start;
    size_t content_length;
    size_t capacity;
    unsigned char *buffer;
} mbedtls_test_ssl_buffer;

/*
 * Context for a message metadata queue (fifo) that is on top of the ring buffer.
 */
typedef struct mbedtls_test_ssl_message_queue {
    size_t *messages;
    int pos;
    int num;
    int capacity;
} mbedtls_test_ssl_message_queue;

/*
 * Context for the I/O callbacks simulating network connection.
 */

#define MBEDTLS_MOCK_SOCKET_CONNECTED 1

typedef struct mbedtls_test_mock_socket {
    int status;
    mbedtls_test_ssl_buffer *input;
    mbedtls_test_ssl_buffer *output;
    struct mbedtls_test_mock_socket *peer;
} mbedtls_test_mock_socket;

/* Errors used in the message socket mocks */

#define MBEDTLS_TEST_ERROR_CONTEXT_ERROR -55
#define MBEDTLS_TEST_ERROR_SEND_FAILED -66
#define MBEDTLS_TEST_ERROR_RECV_FAILED -77

/*
 * Structure used as an addon, or a wrapper, around the mocked sockets.
 * Contains an input queue, to which the other socket pushes metadata,
 * and an output queue, to which this one pushes metadata. This context is
 * considered as an owner of the input queue only, which is initialized and
 * freed in the respective setup and free calls.
 */
typedef struct mbedtls_test_message_socket_context {
    mbedtls_test_ssl_message_queue *queue_input;
    mbedtls_test_ssl_message_queue *queue_output;
    mbedtls_test_mock_socket *socket;
} mbedtls_test_message_socket_context;

#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED)

/*
 * Endpoint structure for SSL communication tests.
 */
typedef struct mbedtls_test_ssl_endpoint {
    const char *name;
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;
    mbedtls_test_mock_socket socket;
    uintptr_t user_data_cookie; /* A unique value associated with this endpoint */

    /* Objects only used by DTLS.
     * They should be guarded by MBEDTLS_SSL_PROTO_DTLS, but
     * currently aren't because some code accesses them without guards. */
    mbedtls_test_message_socket_context dtls_context;
#if defined(MBEDTLS_TIMING_C)
    mbedtls_timing_delay_context timer;
#endif

    /* Objects owned by the endpoint */
    int *ciphersuites;
    mbedtls_test_ssl_message_queue queue_input;
    mbedtls_x509_crt *ca_chain;
    mbedtls_x509_crt *cert;
    mbedtls_pk_context *pkey;
} mbedtls_test_ssl_endpoint;

#endif /* MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED */

/*
 * Random number generator aimed for TLS unitary tests. Its main purpose is to
 * simplify the set-up of a random number generator for TLS
 * unitary tests: no need to set up a good entropy source for example.
 */
int mbedtls_test_random(void *p_rng, unsigned char *output, size_t output_len);

/*
 * This function can be passed to mbedtls to receive output logs from it. In
 * this case, it will count the instances of a mbedtls_test_ssl_log_pattern
 * in the received logged messages.
 */
void mbedtls_test_ssl_log_analyzer(void *ctx, int level,
                                   const char *file, int line,
                                   const char *str);

void mbedtls_test_init_handshake_options(
    mbedtls_test_handshake_test_options *opts);

void mbedtls_test_free_handshake_options(
    mbedtls_test_handshake_test_options *opts);

/*
 * Initialises \p buf. After calling this function it is safe to call
 * `mbedtls_test_ssl_buffer_free()` on \p buf.
 */
void mbedtls_test_ssl_buffer_init(mbedtls_test_ssl_buffer *buf);

/*
 * Sets up \p buf. After calling this function it is safe to call
 * `mbedtls_test_ssl_buffer_put()` and `mbedtls_test_ssl_buffer_get()`
 * on \p buf.
 */
int mbedtls_test_ssl_buffer_setup(mbedtls_test_ssl_buffer *buf,
                                  size_t capacity);

void mbedtls_test_ssl_buffer_free(mbedtls_test_ssl_buffer *buf);

/*
 * Puts \p input_len bytes from the \p input buffer into the ring buffer \p buf.
 *
 * \p buf must have been initialized and set up by calling
 * `mbedtls_test_ssl_buffer_init()` and `mbedtls_test_ssl_buffer_setup()`.
 *
 * \retval  \p input_len, if the data fits.
 * \retval  0 <= value < \p input_len, if the data does not fit.
 * \retval  -1, if \p buf is NULL, it hasn't been set up or \p input_len is not
 *          zero and \p input is NULL.
 */
int mbedtls_test_ssl_buffer_put(mbedtls_test_ssl_buffer *buf,
                                const unsigned char *input, size_t input_len);

/*
 * Gets \p output_len bytes from the ring buffer \p buf into the
 * \p output buffer. The output buffer can be NULL, in this case a part of the
 * ring buffer will be dropped, if the requested length is available.
 *
 * \p buf must have been initialized and set up by calling
 * `mbedtls_test_ssl_buffer_init()` and `mbedtls_test_ssl_buffer_setup()`.
 *
 * \retval  \p output_len, if the data is available.
 * \retval  0 <= value < \p output_len, if the data is not available.
 * \retval  -1, if \buf is NULL or it hasn't been set up.
 */
int mbedtls_test_ssl_buffer_get(mbedtls_test_ssl_buffer *buf,
                                unsigned char *output, size_t output_len);

/*
 * Errors used in the message transport mock tests
 */
 #define MBEDTLS_TEST_ERROR_ARG_NULL -11
 #define MBEDTLS_TEST_ERROR_MESSAGE_TRUNCATED -44

/*
 * Setup and free functions for the message metadata queue.
 *
 * \p capacity describes the number of message metadata chunks that can be held
 *    within the queue.
 *
 * \retval  0, if a metadata queue of a given length can be allocated.
 * \retval  MBEDTLS_ERR_SSL_ALLOC_FAILED, if allocation failed.
 */
int mbedtls_test_ssl_message_queue_setup(
    mbedtls_test_ssl_message_queue *queue, size_t capacity);

void mbedtls_test_ssl_message_queue_free(
    mbedtls_test_ssl_message_queue *queue);

/*
 * Push message length information onto the message metadata queue.
 * This will become the last element to leave it (fifo).
 *
 * \retval  MBEDTLS_TEST_ERROR_ARG_NULL, if the queue is null.
 * \retval  MBEDTLS_ERR_SSL_WANT_WRITE, if the queue is full.
 * \retval  \p len, if the push was successful.
 */
int mbedtls_test_ssl_message_queue_push_info(
    mbedtls_test_ssl_message_queue *queue, size_t len);

/*
 * Pop information about the next message length from the queue. This will be
 * the oldest inserted message length(fifo). \p msg_len can be null, in which
 * case the data will be popped from the queue but not copied anywhere.
 *
 * \retval  MBEDTLS_TEST_ERROR_ARG_NULL, if the queue is null.
 * \retval  MBEDTLS_ERR_SSL_WANT_READ, if the queue is empty.
 * \retval  message length, if the pop was successful, up to the given
            \p buf_len.
 */
int mbedtls_test_ssl_message_queue_pop_info(
    mbedtls_test_ssl_message_queue *queue, size_t buf_len);

/*
 * Setup and teardown functions for mock sockets.
 */
void mbedtls_test_mock_socket_init(mbedtls_test_mock_socket *socket);

/*
 * Closes the socket \p socket.
 *
 * \p socket must have been previously initialized by calling
 * mbedtls_test_mock_socket_init().
 *
 * This function frees all allocated resources and both sockets are aware of the
 * new connection state.
 *
 * That is, this function does not simulate half-open TCP connections and the
 * phenomenon that when closing a UDP connection the peer is not aware of the
 * connection having been closed.
 */
void mbedtls_test_mock_socket_close(mbedtls_test_mock_socket *socket);

/*
 * Establishes a connection between \p peer1 and \p peer2.
 *
 * \p peer1 and \p peer2 must have been previously initialized by calling
 * mbedtls_test_mock_socket_init().
 *
 * The capacities of the internal buffers are set to \p bufsize. Setting this to
 * the correct value allows for simulation of MTU, sanity testing the mock
 * implementation and mocking TCP connections with lower memory cost.
 */
int mbedtls_test_mock_socket_connect(mbedtls_test_mock_socket *peer1,
                                     mbedtls_test_mock_socket *peer2,
                                     size_t bufsize);


/*
 * Callbacks for simulating blocking I/O over connection-oriented transport.
 */
int mbedtls_test_mock_tcp_send_b(void *ctx,
                                 const unsigned char *buf, size_t len);

int mbedtls_test_mock_tcp_recv_b(void *ctx, unsigned char *buf, size_t len);

/*
 * Callbacks for simulating non-blocking I/O over connection-oriented transport.
 */
int mbedtls_test_mock_tcp_send_nb(void *ctx,
                                  const unsigned char *buf, size_t len);

int mbedtls_test_mock_tcp_recv_nb(void *ctx, unsigned char *buf, size_t len);

void mbedtls_test_message_socket_init(
    mbedtls_test_message_socket_context *ctx);

/*
 * Setup a given message socket context including initialization of
 * input/output queues to a chosen capacity of messages. Also set the
 * corresponding mock socket.
 *
 * \retval  0, if everything succeeds.
 * \retval  MBEDTLS_ERR_SSL_ALLOC_FAILED, if allocation of a message
 *          queue failed.
 */
int mbedtls_test_message_socket_setup(
    mbedtls_test_ssl_message_queue *queue_input,
    mbedtls_test_ssl_message_queue *queue_output,
    size_t queue_capacity,
    mbedtls_test_mock_socket *socket,
    mbedtls_test_message_socket_context *ctx);

/*
 * Close a given message socket context, along with the socket itself. Free the
 * memory allocated by the input queue.
 */
void mbedtls_test_message_socket_close(
    mbedtls_test_message_socket_context *ctx);

/*
 * Send one message through a given message socket context.
 *
 * \retval  \p len, if everything succeeds.
 * \retval  MBEDTLS_TEST_ERROR_CONTEXT_ERROR, if any of the needed context
 *          elements or the context itself is null.
 * \retval  MBEDTLS_TEST_ERROR_SEND_FAILED if
 *          mbedtls_test_mock_tcp_send_b failed.
 * \retval  MBEDTLS_ERR_SSL_WANT_WRITE, if the output queue is full.
 *
 * This function will also return any error from
 * mbedtls_test_ssl_message_queue_push_info.
 */
int mbedtls_test_mock_tcp_send_msg(void *ctx,
                                   const unsigned char *buf, size_t len);

/*
 * Receive one message from a given message socket context and return message
 * length or an error.
 *
 * \retval  message length, if everything succeeds.
 * \retval  MBEDTLS_TEST_ERROR_CONTEXT_ERROR, if any of the needed context
 *          elements or the context itself is null.
 * \retval  MBEDTLS_TEST_ERROR_RECV_FAILED if
 *          mbedtls_test_mock_tcp_recv_b failed.
 *
 * This function will also return any error other than
 * MBEDTLS_TEST_ERROR_MESSAGE_TRUNCATED from test_ssl_message_queue_peek_info.
 */
int mbedtls_test_mock_tcp_recv_msg(void *ctx,
                                   unsigned char *buf, size_t buf_len);

#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED)

/*
 * Load default CA certificates and endpoint keys into \p ep.
 *
 * \retval  0 on success, otherwise error code.
 */
int mbedtls_test_ssl_endpoint_certificate_init(mbedtls_test_ssl_endpoint *ep,
                                               int pk_alg,
                                               int opaque_alg, int opaque_alg2,
                                               int opaque_usage);

/** Initialize the configuration in an SSL endpoint structure.
 *
 * \note You must call `mbedtls_test_ssl_endpoint_free()` after
 * calling this function, even if it fails. This is necessary to
 * free data that may have been stored in the endpoint structure.
 *
 * \param[out] ep       The endpoint structure to configure.
 * \param endpoint_type #MBEDTLS_SSL_IS_SERVER or #MBEDTLS_SSL_IS_CLIENT.
 * \param[in] options   The options to use for configuring the endpoint
 *                      structure.
 *
 * \retval  0 on success, otherwise error code.
 */
int mbedtls_test_ssl_endpoint_init_conf(
    mbedtls_test_ssl_endpoint *ep, int endpoint_type,
    const mbedtls_test_handshake_test_options *options);

/** Initialize the session context in an endpoint structure.
 *
 * \note The endpoint structure must have been set up with
 *       mbedtls_test_ssl_endpoint_init_conf() with the same \p options.
 *       Between calling mbedtls_test_ssl_endpoint_init_conf() and
 *       mbedtls_test_ssl_endpoint_init_ssl(), you may configure `ep->ssl`
 *       further if you know what you're doing.
 *
 * \note You must call `mbedtls_test_ssl_endpoint_free()` after
 * calling this function, even if it fails. This is necessary to
 * free data that may have been stored in the endpoint structure.
 *
 * \param[out] ep       The endpoint structure to set up.
 * \param[in] options   The options used for configuring the endpoint
 *                      structure.
 *
 * \retval  0 on success, otherwise error code.
 */
int mbedtls_test_ssl_endpoint_init_ssl(
    mbedtls_test_ssl_endpoint *ep,
    const mbedtls_test_handshake_test_options *options);

/** Initialize the configuration and a context in an SSL endpoint structure.
 *
 * This function is equivalent to calling
 * mbedtls_test_ssl_endpoint_init_conf() followed by
 * mbedtls_test_ssl_endpoint_init_ssl().
 *
 * \note You must call `mbedtls_test_ssl_endpoint_free()` after
 * calling this function, even if it fails. This is necessary to
 * free data that may have been stored in the endpoint structure.
 *
 * \param[out] ep       The endpoint structure to configure.
 * \param endpoint_type #MBEDTLS_SSL_IS_SERVER or #MBEDTLS_SSL_IS_CLIENT.
 * \param[in] options   The options to use for configuring the endpoint
 *                      structure.
 *
 * \retval  0 on success, otherwise error code.
 */
int mbedtls_test_ssl_endpoint_init(
    mbedtls_test_ssl_endpoint *ep, int endpoint_type,
    const mbedtls_test_handshake_test_options *options);

/*
 * Deinitializes endpoint represented by \p ep.
 */
void mbedtls_test_ssl_endpoint_free(mbedtls_test_ssl_endpoint *ep);

/* Join a DTLS client with a DTLS server.
 *
 * You must call this function after setting up the endpoint objects
 * and before starting a DTLS handshake.
 *
 * \param client    The client. It must have been set up with
 *                  mbedtls_test_ssl_endpoint_init().
 * \param server    The server. It must have been set up with
 *                  mbedtls_test_ssl_endpoint_init().
 *
 * \retval  0 on success, otherwise error code.
 */
int mbedtls_test_ssl_dtls_join_endpoints(mbedtls_test_ssl_endpoint *client,
                                         mbedtls_test_ssl_endpoint *server);

/*
 * This function moves ssl handshake from \p ssl to prescribed \p state.
 * /p second_ssl is used as second endpoint and their sockets have to be
 * connected before calling this function.
 *
 * For example, to perform a full handshake:
 * ```
 * mbedtls_test_move_handshake_to_state(
 *                       &server.ssl, &client.ssl,
 *                       MBEDTLS_SSL_HANDSHAKE_OVER);
 * mbedtls_test_move_handshake_to_state(
 *                       &client.ssl, &server.ssl,
 *                       MBEDTLS_SSL_HANDSHAKE_OVER);
 * ```
 * Note that you need both calls to reach the handshake-over state on
 * both sides.
 *
 * \retval  0 on success, otherwise error code.
 */
int mbedtls_test_move_handshake_to_state(mbedtls_ssl_context *ssl,
                                         mbedtls_ssl_context *second_ssl,
                                         int state);

#endif /* MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED */

/*
 * Helper function setting up inverse record transformations
 * using given cipher, hash, EtM mode, authentication tag length,
 * and version.
 */
#define CHK(x)                                  \
    do                                          \
    {                                           \
        if (!(x))                               \
        {                                       \
            ret = -1;                           \
            goto cleanup;                       \
        }                                       \
    } while (0)

#if MBEDTLS_SSL_CID_OUT_LEN_MAX > MBEDTLS_SSL_CID_IN_LEN_MAX
#define SSL_CID_LEN_MIN MBEDTLS_SSL_CID_IN_LEN_MAX
#else
#define SSL_CID_LEN_MIN MBEDTLS_SSL_CID_OUT_LEN_MAX
#endif

#if defined(MBEDTLS_SSL_PROTO_TLS1_2) && \
    defined(PSA_WANT_ALG_CBC_NO_PADDING) && defined(PSA_WANT_KEY_TYPE_AES)
int mbedtls_test_psa_cipher_encrypt_helper(mbedtls_ssl_transform *transform,
                                           const unsigned char *iv,
                                           size_t iv_len,
                                           const unsigned char *input,
                                           size_t ilen,
                                           unsigned char *output,
                                           size_t *olen);
#endif /* MBEDTLS_SSL_PROTO_TLS1_2 && PSA_WANT_ALG_CBC_NO_PADDING &&
          PSA_WANT_KEY_TYPE_AES */

int mbedtls_test_ssl_build_transforms(mbedtls_ssl_transform *t_in,
                                      mbedtls_ssl_transform *t_out,
                                      int cipher_type, int hash_id,
                                      int etm, int tag_mode,
                                      mbedtls_ssl_protocol_version tls_version,
                                      size_t cid0_len,
                                      size_t cid1_len);

#if defined(MBEDTLS_SSL_SOME_SUITES_USE_MAC)
/**
 * \param[in,out] record        The record to prepare.
 *                              It must contain the data to MAC at offset
 *                              `record->data_offset`, of length
 *                              `record->data_length`.
 *                              On success, write the MAC immediately
 *                              after the data and increment
 *                              `record->data_length` accordingly.
 * \param[in,out] transform_out The out transform, typically prepared by
 *                              mbedtls_test_ssl_build_transforms().
 *                              Its HMAC context may be used. Other than that
 *                              it is treated as an input parameter.
 *
 * \return                      0 on success, an `MBEDTLS_ERR_xxx` error code
 *                              or -1 on error.
 */
int mbedtls_test_ssl_prepare_record_mac(mbedtls_record *record,
                                        mbedtls_ssl_transform *transform_out);
#endif /* MBEDTLS_SSL_SOME_SUITES_USE_MAC */

/*
 * Populate a session structure for serialization tests.
 * Choose dummy values, mostly non-0 to distinguish from the init default.
 */
int mbedtls_test_ssl_tls12_populate_session(mbedtls_ssl_session *session,
                                            int ticket_len,
                                            int endpoint_type,
                                            const char *crt_file);

#if defined(MBEDTLS_SSL_PROTO_TLS1_3)
int mbedtls_test_ssl_tls13_populate_session(mbedtls_ssl_session *session,
                                            int ticket_len,
                                            int endpoint_type);
#endif /* MBEDTLS_SSL_PROTO_TLS1_3 */

/*
 * Perform data exchanging between \p ssl_1 and \p ssl_2 and check if the
 * message was sent in the correct number of fragments.
 *
 * /p ssl_1 and /p ssl_2    Endpoints represented by mbedtls_ssl_context. Both
 *                          of them must be initialized and connected
 *                          beforehand.
 * /p msg_len_1 and /p msg_len_2 specify the size of the message to send.
 * /p expected_fragments_1 and /p expected_fragments_2 determine in how many
 *                          fragments the message should be sent.
 *      expected_fragments is 0: can be used for DTLS testing while the message
 *                          size is larger than MFL. In that case the message
 *                          cannot be fragmented and sent to the second
 *                          endpoint.
 *                          This value can be used for negative tests.
 *      expected_fragments is 1: can be used for TLS/DTLS testing while the
 *                          message size is below MFL
 *      expected_fragments > 1: can be used for TLS testing while the message
 *                          size is larger than MFL
 *
 * \retval  0 on success, otherwise error code.
 */
int mbedtls_test_ssl_exchange_data(
    mbedtls_ssl_context *ssl_1,
    int msg_len_1, const int expected_fragments_1,
    mbedtls_ssl_context *ssl_2,
    int msg_len_2, const int expected_fragments_2);

#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED)
int mbedtls_test_ssl_do_handshake_with_endpoints(
    mbedtls_test_ssl_endpoint *server_ep,
    mbedtls_test_ssl_endpoint *client_ep,
    mbedtls_test_handshake_test_options *options,
    mbedtls_ssl_protocol_version proto);
#endif /* defined(MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED) */

#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED)
/** Perform an SSL handshake and exchange data over the connection.
 *
 * This function also handles cases where the handshake is expected to fail.
 *
 * If the handshake succeeds as expected, this function validates that
 * connection parameters are as expected, exchanges data over the
 * connection, and exercises some optional protocol features if they
 * are enabled. See the code to see what features are validated and exercised.
 *
 * The handshake is expected to fail in the following cases:
 * - If `options->expected_handshake_result != 0`.
 * - If `options->expected_negotiated_version == MBEDTLS_SSL_VERSION_UNKNOWN`.
 *
 * \param[in] options   Options for the connection.
 * \param client        The client endpoint. It must have been set up with
 *                      mbedtls_test_ssl_endpoint_init() with \p options
 *                      and #MBEDTLS_SSL_IS_CLIENT.
 * \param server        The server endpoint. It must have been set up with
 *                      mbedtls_test_ssl_endpoint_init() with \p options
 *                      and #MBEDTLS_SSL_IS_CLIENT.
 *
 * \return              1 on success, 0 on failure. On failure, this function
 *                      calls mbedtls_test_fail(), indicating the failure
 *                      reason and location. The causes of failure are:
 *                      - Inconsistent options or bad endpoint state.
 *                      - Operational problem during the handshake.
 *                      - The handshake was expected to pass, but failed.
 *                      - The handshake was expected to fail, but passed or
 *                        failed with a different result.
 *                      - The handshake passed as expected, but some connection
 *                        parameter (e.g. protocol version, cipher suite, ...)
 *                        is not as expected.
 *                      - The handshake passed as expected, but something
 *                        went wrong when attempting to exchange data.
 *                      - The handshake passed as expected, but something
 *                        went wrong when exercising other features
 *                        (e.g. renegotiation, serialization, ...).
 */
int mbedtls_test_ssl_perform_connection(
    const mbedtls_test_handshake_test_options *options,
    mbedtls_test_ssl_endpoint *client,
    mbedtls_test_ssl_endpoint *server);

void mbedtls_test_ssl_perform_handshake(
    const mbedtls_test_handshake_test_options *options);
#endif /* MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED */

#if defined(MBEDTLS_TEST_HOOKS)
/*
 * Tweak vector lengths in a TLS 1.3 Certificate message
 *
 * \param[in]       buf    Buffer containing the Certificate message to tweak
 * \param[in]]out]  end    End of the buffer to parse
 * \param           tweak  Tweak identifier (from 1 to the number of tweaks).
 * \param[out]  expected_result  Error code expected from the parsing function
 * \param[out]  args  Arguments of the MBEDTLS_SSL_CHK_BUF_READ_PTR call that
 *                    is expected to fail. All zeroes if no
 *                    MBEDTLS_SSL_CHK_BUF_READ_PTR failure is expected.
 */
int mbedtls_test_tweak_tls13_certificate_msg_vector_len(
    unsigned char *buf, unsigned char **end, int tweak,
    int *expected_result, mbedtls_ssl_chk_buf_ptr_args *args);
#endif /* MBEDTLS_TEST_HOOKS */

#if defined(MBEDTLS_SSL_SESSION_TICKETS)
int mbedtls_test_ticket_write(
    void *p_ticket, const mbedtls_ssl_session *session,
    unsigned char *start, const unsigned char *end,
    size_t *tlen, uint32_t *ticket_lifetime);

int mbedtls_test_ticket_parse(void *p_ticket, mbedtls_ssl_session *session,
                              unsigned char *buf, size_t len);
#endif /* MBEDTLS_SSL_SESSION_TICKETS */

#if defined(MBEDTLS_SSL_CLI_C) && defined(MBEDTLS_SSL_SRV_C) && \
    defined(MBEDTLS_SSL_PROTO_TLS1_3) && defined(MBEDTLS_SSL_SESSION_TICKETS) && \
    defined(MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED)
int mbedtls_test_get_tls13_ticket(
    mbedtls_test_handshake_test_options *client_options,
    mbedtls_test_handshake_test_options *server_options,
    mbedtls_ssl_session *session);
#endif

#define ECJPAKE_TEST_PWD        "bla"

#define ECJPAKE_TEST_SET_PASSWORD(exp_ret_val)                            \
    ret = (use_opaque_arg) ?                                              \
          mbedtls_ssl_set_hs_ecjpake_password_opaque(&ssl, pwd_slot) :    \
          mbedtls_ssl_set_hs_ecjpake_password(&ssl, pwd_string, pwd_len); \
    TEST_EQUAL(ret, exp_ret_val)

#define TEST_AVAILABLE_ECC(tls_id_, group_id_, psa_family_, psa_bits_)   \
    TEST_EQUAL(mbedtls_ssl_get_ecp_group_id_from_tls_id(tls_id_),        \
               group_id_);                                               \
    TEST_EQUAL(mbedtls_ssl_get_tls_id_from_ecp_group_id(group_id_),      \
               tls_id_);                                                 \
    TEST_EQUAL(mbedtls_ssl_get_psa_curve_info_from_tls_id(tls_id_,       \
                                                          &psa_type, &psa_bits), PSA_SUCCESS);                \
    TEST_EQUAL(psa_family_, PSA_KEY_TYPE_ECC_GET_FAMILY(psa_type));    \
    TEST_EQUAL(psa_bits_, psa_bits);

#define TEST_UNAVAILABLE_ECC(tls_id_, group_id_, psa_family_, psa_bits_) \
    TEST_EQUAL(mbedtls_ssl_get_ecp_group_id_from_tls_id(tls_id_),        \
               MBEDTLS_ECP_DP_NONE);                                     \
    TEST_EQUAL(mbedtls_ssl_get_tls_id_from_ecp_group_id(group_id_),      \
               0);                                                       \
    TEST_EQUAL(mbedtls_ssl_get_psa_curve_info_from_tls_id(tls_id_,       \
                                                          &psa_type, &psa_bits), \
               PSA_ERROR_NOT_SUPPORTED);

#endif /* MBEDTLS_SSL_TLS_C */

#endif /* SSL_HELPERS_H */
