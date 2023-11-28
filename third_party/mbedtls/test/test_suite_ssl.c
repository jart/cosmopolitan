/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#include "third_party/mbedtls/test/test.inc"
#include "third_party/mbedtls/ssl_invasive.h"
#include "libc/testlib/testlib.h"
#include "libc/log/log.h"
#include "libc/time/time.h"
#include "libc/stdio/rand.h"
#include "libc/intrin/safemacros.internal.h"
#include "third_party/mbedtls/test/test.inc"
/*
 * *** THIS FILE WAS MACHINE GENERATED ***
 *
 * This file has been machine generated using the script:
 * generate_test_code.py and then mbedtls_test_suite.sh and then mbedtls_test_suite.sh
 *
 * Test file      : ./test_suite_ssl.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_ssl.function
 *      Test suite data     : suites/test_suite_ssl.data
 *
 */


#define TEST_SUITE_ACTIVE

#if defined(MBEDTLS_SSL_TLS_C)
#include "third_party/mbedtls/ssl.h"
#include "third_party/mbedtls/ssl_internal.h"
#include "third_party/mbedtls/ctr_drbg.h"
#include "third_party/mbedtls/entropy.h"
#include "third_party/mbedtls/certs.h"
#include "third_party/mbedtls/debug.h"
#include "third_party/mbedtls/ssl_tls13_keys.h"
enum
{
#define MBEDTLS_SSL_TLS1_3_LABEL( name, string )   \
     tls1_3_label_ ## name,
MBEDTLS_SSL_TLS1_3_LABEL_LIST
#undef MBEDTLS_SSL_TLS1_3_LABEL
};

typedef struct log_pattern
{
    const char *pattern;
    size_t counter;
} log_pattern;

/*
 * This function can be passed to mbedtls to receive output logs from it. In
 * this case, it will count the instances of a log_pattern in the received
 * logged messages.
 */
void log_analyzer( void *ctx, int level,
                   const char *file, int line,
                   const char *str )
{
    log_pattern *p = (log_pattern *) ctx;

    (void) level;
    (void) line;
    (void) file;

    if( NULL != p &&
        NULL != p->pattern &&
        NULL != strstr( str, p->pattern ) )
    {
        p->counter++;
    }
}

/* Invalid minor version used when not specifying a min/max version or expecting a test to fail */
#define TEST_SSL_MINOR_VERSION_NONE -1

typedef struct handshake_test_options
{
    const char *cipher;
    int client_min_version;
    int client_max_version;
    int server_min_version;
    int server_max_version;
    int expected_negotiated_version;
    int pk_alg;
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
} handshake_test_options;

void init_handshake_options( handshake_test_options *opts )
{
  opts->cipher = "";
  opts->client_min_version = TEST_SSL_MINOR_VERSION_NONE;
  opts->client_max_version = TEST_SSL_MINOR_VERSION_NONE;
  opts->server_min_version = TEST_SSL_MINOR_VERSION_NONE;
  opts->server_max_version = TEST_SSL_MINOR_VERSION_NONE;
  opts->expected_negotiated_version = MBEDTLS_SSL_MINOR_VERSION_3;
  opts->pk_alg = MBEDTLS_PK_RSA;
  opts->psk_str = NULL;
  opts->dtls = 0;
  opts->srv_auth_mode = MBEDTLS_SSL_VERIFY_NONE;
  opts->serialize = 0;
  opts->mfl = MBEDTLS_SSL_MAX_FRAG_LEN_NONE;
  opts->cli_msg_len = 100;
  opts->srv_msg_len = 100;
  opts->expected_cli_fragments = 1;
  opts->expected_srv_fragments = 1;
  opts->renegotiate = 0;
  opts->legacy_renegotiation = MBEDTLS_SSL_LEGACY_NO_RENEGOTIATION;
  opts->srv_log_obj = NULL;
  opts->srv_log_obj = NULL;
  opts->srv_log_fun = NULL;
  opts->cli_log_fun = NULL;
  opts->resize_buffers = 1;
}
/*
 * Buffer structure for custom I/O callbacks.
 */

typedef struct mbedtls_test_buffer
{
    size_t start;
    size_t content_length;
    size_t capacity;
    unsigned char *buffer;
} mbedtls_test_buffer;

/*
 * Initialises \p buf. After calling this function it is safe to call
 * `mbedtls_test_buffer_free()` on \p buf.
 */
void mbedtls_test_buffer_init( mbedtls_test_buffer *buf )
{
    memset( buf, 0, sizeof( *buf ) );
}

/*
 * Sets up \p buf. After calling this function it is safe to call
 * `mbedtls_test_buffer_put()` and `mbedtls_test_buffer_get()` on \p buf.
 */
int mbedtls_test_buffer_setup( mbedtls_test_buffer *buf, size_t capacity )
{
    buf->buffer = (unsigned char*) mbedtls_calloc( capacity,
                                                   sizeof(unsigned char) );
    if( NULL == buf->buffer )
        return MBEDTLS_ERR_SSL_ALLOC_FAILED;
    buf->capacity = capacity;

    return 0;
}

void mbedtls_test_buffer_free( mbedtls_test_buffer *buf )
{
    if( buf->buffer != NULL )
        mbedtls_free( buf->buffer );

    memset( buf, 0, sizeof( *buf ) );
}

/*
 * Puts \p input_len bytes from the \p input buffer into the ring buffer \p buf.
 *
 * \p buf must have been initialized and set up by calling
 * `mbedtls_test_buffer_init()` and `mbedtls_test_buffer_setup()`.
 *
 * \retval  \p input_len, if the data fits.
 * \retval  0 <= value < \p input_len, if the data does not fit.
 * \retval  -1, if \p buf is NULL, it hasn't been set up or \p input_len is not
 *          zero and \p input is NULL.
 */
int mbedtls_test_buffer_put( mbedtls_test_buffer *buf,
                             const unsigned char *input, size_t input_len )
{
    size_t overflow = 0;

    if( ( buf == NULL ) || ( buf->buffer == NULL ) )
        return -1;

    /* Reduce input_len to a number that fits in the buffer. */
    if ( ( buf->content_length + input_len ) > buf->capacity )
    {
        input_len = buf->capacity - buf->content_length;
    }

    if( input == NULL )
    {
        return ( input_len == 0 ) ? 0 : -1;
    }

     /* Check if the buffer has not come full circle and free space is not in
      * the middle */
    if( buf->start + buf->content_length < buf->capacity )
    {

        /* Calculate the number of bytes that need to be placed at lower memory
        * address */
        if( buf->start + buf->content_length + input_len
            > buf->capacity )
        {
            overflow = ( buf->start + buf->content_length + input_len )
                        % buf->capacity;
        }

        memcpy( buf->buffer + buf->start + buf->content_length, input,
                    input_len - overflow );
        memcpy( buf->buffer, input + input_len - overflow, overflow );

    }
    else
    {
        /* The buffer has come full circle and free space is in the middle */
        memcpy( buf->buffer + buf->start + buf->content_length - buf->capacity,
                input, input_len );
    }

    buf->content_length += input_len;
    return input_len;
}

/*
 * Gets \p output_len bytes from the ring buffer \p buf into the
 * \p output buffer. The output buffer can be NULL, in this case a part of the
 * ring buffer will be dropped, if the requested length is available.
 *
 * \p buf must have been initialized and set up by calling
 * `mbedtls_test_buffer_init()` and `mbedtls_test_buffer_setup()`.
 *
 * \retval  \p output_len, if the data is available.
 * \retval  0 <= value < \p output_len, if the data is not available.
 * \retval  -1, if \buf is NULL or it hasn't been set up.
 */
int mbedtls_test_buffer_get( mbedtls_test_buffer *buf,
                             unsigned char* output, size_t output_len )
{
    size_t overflow = 0;

    if( ( buf == NULL ) || ( buf->buffer == NULL ) )
        return -1;

    if( output == NULL && output_len == 0 )
        return 0;

    if( buf->content_length < output_len )
        output_len = buf->content_length;

    /* Calculate the number of bytes that need to be drawn from lower memory
     * address */
    if( buf->start + output_len > buf->capacity )
    {
        overflow = ( buf->start + output_len ) % buf->capacity;
    }

    if( output != NULL )
    {
        memcpy( output, buf->buffer + buf->start, output_len - overflow );
        memcpy( output + output_len - overflow, buf->buffer, overflow );
    }

    buf->content_length -= output_len;
    buf->start = ( buf->start + output_len ) % buf->capacity;

    return output_len;
}

/*
 * Errors used in the message transport mock tests
 */
 #define MBEDTLS_TEST_ERROR_ARG_NULL -11
 #define MBEDTLS_TEST_ERROR_MESSAGE_TRUNCATED -44

/*
 * Context for a message metadata queue (fifo) that is on top of the ring buffer.
 */
typedef struct mbedtls_test_message_queue
{
    size_t *messages;
    int pos;
    int num;
    int capacity;
} mbedtls_test_message_queue;

/*
 * Setup and free functions for the message metadata queue.
 *
 * \p capacity describes the number of message metadata chunks that can be held
 *    within the queue.
 *
 * \retval  0, if a metadata queue of a given length can be allocated.
 * \retval  MBEDTLS_ERR_SSL_ALLOC_FAILED, if allocation failed.
 */
int mbedtls_test_message_queue_setup( mbedtls_test_message_queue *queue,
                                      size_t capacity )
{
    queue->messages = (size_t*) mbedtls_calloc( capacity, sizeof(size_t) );
    if( NULL == queue->messages )
        return MBEDTLS_ERR_SSL_ALLOC_FAILED;

    queue->capacity = capacity;
    queue->pos = 0;
    queue->num = 0;

    return 0;
}

void mbedtls_test_message_queue_free( mbedtls_test_message_queue *queue )
{
    if( queue == NULL )
        return;

    if( queue->messages != NULL )
        mbedtls_free( queue->messages );

    memset( queue, 0, sizeof( *queue ) );
}

/*
 * Push message length information onto the message metadata queue.
 * This will become the last element to leave it (fifo).
 *
 * \retval  MBEDTLS_TEST_ERROR_ARG_NULL, if the queue is null.
 * \retval  MBEDTLS_ERR_SSL_WANT_WRITE, if the queue is full.
 * \retval  \p len, if the push was successful.
 */
int mbedtls_test_message_queue_push_info( mbedtls_test_message_queue *queue,
                                          size_t len )
{
    int place;
    if( queue == NULL )
        return MBEDTLS_TEST_ERROR_ARG_NULL;

    if( queue->num >= queue->capacity )
        return MBEDTLS_ERR_SSL_WANT_WRITE;

    place = ( queue->pos + queue->num ) % queue->capacity;
    queue->messages[place] = len;
    queue->num++;
    return len;
}

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
int mbedtls_test_message_queue_pop_info( mbedtls_test_message_queue *queue,
                                         size_t buf_len )
{
    size_t message_length;
    if( queue == NULL )
        return MBEDTLS_TEST_ERROR_ARG_NULL;
    if( queue->num == 0 )
        return MBEDTLS_ERR_SSL_WANT_READ;

    message_length = queue->messages[queue->pos];
    queue->messages[queue->pos] = 0;
    queue->num--;
    queue->pos++;
    queue->pos %= queue->capacity;
    if( queue->pos < 0 )
        queue->pos += queue->capacity;

    return ( message_length > buf_len ) ? buf_len : message_length;
}

/*
 * Take a peek on the info about the next message length from the queue.
 * This will be the oldest inserted message length(fifo).
 *
 * \retval  MBEDTLS_TEST_ERROR_ARG_NULL, if the queue is null.
 * \retval  MBEDTLS_ERR_SSL_WANT_READ, if the queue is empty.
 * \retval  0, if the peek was successful.
 * \retval  MBEDTLS_TEST_ERROR_MESSAGE_TRUNCATED, if the given buffer length is
 *          too small to fit the message. In this case the \p msg_len will be
 *          set to the full message length so that the
 *          caller knows what portion of the message can be dropped.
 */
int mbedtls_test_message_queue_peek_info( mbedtls_test_message_queue *queue,
                                          size_t buf_len, size_t* msg_len )
{
    if( queue == NULL || msg_len == NULL )
        return MBEDTLS_TEST_ERROR_ARG_NULL;
    if( queue->num == 0 )
        return MBEDTLS_ERR_SSL_WANT_READ;

    *msg_len = queue->messages[queue->pos];
    return ( *msg_len > buf_len ) ? MBEDTLS_TEST_ERROR_MESSAGE_TRUNCATED : 0;
}
/*
 * Context for the I/O callbacks simulating network connection.
 */

#define MBEDTLS_MOCK_SOCKET_CONNECTED 1

typedef struct mbedtls_mock_socket
{
    int status;
    mbedtls_test_buffer *input;
    mbedtls_test_buffer *output;
    struct mbedtls_mock_socket *peer;
} mbedtls_mock_socket;

/*
 * Setup and teardown functions for mock sockets.
 */
void mbedtls_mock_socket_init( mbedtls_mock_socket *socket )
{
    memset( socket, 0, sizeof( *socket ) );
}

/*
 * Closes the socket \p socket.
 *
 * \p socket must have been previously initialized by calling
 * mbedtls_mock_socket_init().
 *
 * This function frees all allocated resources and both sockets are aware of the
 * new connection state.
 *
 * That is, this function does not simulate half-open TCP connections and the
 * phenomenon that when closing a UDP connection the peer is not aware of the
 * connection having been closed.
 */
void mbedtls_mock_socket_close( mbedtls_mock_socket* socket )
{
    if( socket == NULL )
        return;

    if( socket->input != NULL )
    {
        mbedtls_test_buffer_free( socket->input );
        mbedtls_free( socket->input );
    }

    if( socket->output != NULL )
    {
        mbedtls_test_buffer_free( socket->output );
        mbedtls_free( socket->output );
    }

    if( socket->peer != NULL )
        memset( socket->peer, 0, sizeof( *socket->peer ) );

    memset( socket, 0, sizeof( *socket ) );
}

/*
 * Establishes a connection between \p peer1 and \p peer2.
 *
 * \p peer1 and \p peer2 must have been previously initialized by calling
 * mbedtls_mock_socket_init().
 *
 * The capacites of the internal buffers are set to \p bufsize. Setting this to
 * the correct value allows for simulation of MTU, sanity testing the mock
 * implementation and mocking TCP connections with lower memory cost.
 */
int mbedtls_mock_socket_connect( mbedtls_mock_socket* peer1,
                                 mbedtls_mock_socket* peer2,
                                 size_t bufsize )
{
    int ret = -1;

    peer1->output =
        (mbedtls_test_buffer*) mbedtls_calloc( 1, sizeof(mbedtls_test_buffer) );
    if( peer1->output == NULL )
    {
        ret = MBEDTLS_ERR_SSL_ALLOC_FAILED;
        goto exit;
    }
    mbedtls_test_buffer_init( peer1->output );
    if( 0 != ( ret = mbedtls_test_buffer_setup( peer1->output, bufsize ) ) )
    {
        goto exit;
    }

    peer2->output =
        (mbedtls_test_buffer*) mbedtls_calloc( 1, sizeof(mbedtls_test_buffer) );
    if( peer2->output == NULL )
    {
        ret = MBEDTLS_ERR_SSL_ALLOC_FAILED;
        goto exit;
    }
    mbedtls_test_buffer_init( peer2->output );
    if( 0 != ( ret = mbedtls_test_buffer_setup( peer2->output, bufsize ) ) )
    {
        goto exit;
    }

    peer1->peer = peer2;
    peer2->peer = peer1;
    peer1->input = peer2->output;
    peer2->input = peer1->output;

    peer1->status = peer2->status = MBEDTLS_MOCK_SOCKET_CONNECTED;
    ret = 0;

exit:

    if( ret != 0 )
    {
        mbedtls_mock_socket_close( peer1 );
        mbedtls_mock_socket_close( peer2 );
    }

    return ret;
}

/*
 * Callbacks for simulating blocking I/O over connection-oriented transport.
 */

int mbedtls_mock_tcp_send_b( void *ctx, const unsigned char *buf, size_t len )
{
    mbedtls_mock_socket *socket = (mbedtls_mock_socket*) ctx;

    if( socket == NULL || socket->status != MBEDTLS_MOCK_SOCKET_CONNECTED )
        return -1;

    return mbedtls_test_buffer_put( socket->output, buf, len );
}

int mbedtls_mock_tcp_recv_b( void *ctx, unsigned char *buf, size_t len )
{
    mbedtls_mock_socket *socket = (mbedtls_mock_socket*) ctx;

    if( socket == NULL || socket->status != MBEDTLS_MOCK_SOCKET_CONNECTED )
        return -1;

    return mbedtls_test_buffer_get( socket->input, buf, len );
}

/*
 * Callbacks for simulating non-blocking I/O over connection-oriented transport.
 */

int mbedtls_mock_tcp_send_nb( void *ctx, const unsigned char *buf, size_t len )
{
    mbedtls_mock_socket *socket = (mbedtls_mock_socket*) ctx;

    if( socket == NULL || socket->status != MBEDTLS_MOCK_SOCKET_CONNECTED )
        return -1;

    if( socket->output->capacity == socket->output->content_length )
    {
        return MBEDTLS_ERR_SSL_WANT_WRITE;
    }

    return mbedtls_test_buffer_put( socket->output, buf, len );
}

int mbedtls_mock_tcp_recv_nb( void *ctx, unsigned char *buf, size_t len )
{
    mbedtls_mock_socket *socket = (mbedtls_mock_socket*) ctx;

    if( socket == NULL || socket->status != MBEDTLS_MOCK_SOCKET_CONNECTED )
        return -1;

    if( socket->input->content_length == 0 )
    {
        return MBEDTLS_ERR_SSL_WANT_READ;
    }

    return mbedtls_test_buffer_get( socket->input, buf, len );
}

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
typedef struct mbedtls_test_message_socket_context
{
    mbedtls_test_message_queue* queue_input;
    mbedtls_test_message_queue* queue_output;
    mbedtls_mock_socket* socket_;
} mbedtls_test_message_socket_context;

void mbedtls_message_socket_init( mbedtls_test_message_socket_context *ctx )
{
    ctx->queue_input = NULL;
    ctx->queue_output = NULL;
    ctx->socket_ = NULL;
}

/*
 * Setup a given mesasge socket context including initialization of
 * input/output queues to a chosen capacity of messages. Also set the
 * corresponding mock socket.
 *
 * \retval  0, if everything succeeds.
 * \retval  MBEDTLS_ERR_SSL_ALLOC_FAILED, if allocation of a message
 *          queue failed.
 */
int mbedtls_message_socket_setup( mbedtls_test_message_queue* queue_input,
                                  mbedtls_test_message_queue* queue_output,
                                  size_t queue_capacity,
                                  mbedtls_mock_socket* socket,
                                  mbedtls_test_message_socket_context* ctx )
{
    int ret = mbedtls_test_message_queue_setup( queue_input, queue_capacity );
    if( ret != 0 )
        return ret;
    ctx->queue_input = queue_input;
    ctx->queue_output = queue_output;
    ctx->socket_ = socket;
    mbedtls_mock_socket_init( socket );

    return 0;
}

/*
 * Close a given message socket context, along with the socket itself. Free the
 * memory allocated by the input queue.
 */
void mbedtls_message_socket_close( mbedtls_test_message_socket_context* ctx )
{
    if( ctx == NULL )
        return;

    mbedtls_test_message_queue_free( ctx->queue_input );
    mbedtls_mock_socket_close( ctx->socket_ );
    memset( ctx, 0, sizeof( *ctx ) );
}

/*
 * Send one message through a given message socket context.
 *
 * \retval  \p len, if everything succeeds.
 * \retval  MBEDTLS_TEST_ERROR_CONTEXT_ERROR, if any of the needed context
 *          elements or the context itself is null.
 * \retval  MBEDTLS_TEST_ERROR_SEND_FAILED if mbedtls_mock_tcp_send_b failed.
 * \retval  MBEDTLS_ERR_SSL_WANT_WRITE, if the output queue is full.
 *
 * This function will also return any error from
 * mbedtls_test_message_queue_push_info.
 */
int mbedtls_mock_tcp_send_msg( void *ctx, const unsigned char *buf, size_t len )
{
    mbedtls_test_message_queue* queue;
    mbedtls_mock_socket* socket;
    mbedtls_test_message_socket_context *context = (mbedtls_test_message_socket_context*) ctx;

    if( context == NULL || context->socket_ == NULL
        || context->queue_output == NULL )
    {
        return MBEDTLS_TEST_ERROR_CONTEXT_ERROR;
    }

    queue = context->queue_output;
    socket = context->socket_;

    if( queue->num >= queue->capacity )
        return MBEDTLS_ERR_SSL_WANT_WRITE;

    if( mbedtls_mock_tcp_send_b( socket, buf, len ) != (int) len )
        return MBEDTLS_TEST_ERROR_SEND_FAILED;

    return mbedtls_test_message_queue_push_info( queue, len );
}

/*
 * Receive one message from a given message socket context and return message
 * length or an error.
 *
 * \retval  message length, if everything succeeds.
 * \retval  MBEDTLS_TEST_ERROR_CONTEXT_ERROR, if any of the needed context
 *          elements or the context itself is null.
 * \retval  MBEDTLS_TEST_ERROR_RECV_FAILED if mbedtls_mock_tcp_recv_b failed.
 *
 * This function will also return any error other than
 * MBEDTLS_TEST_ERROR_MESSAGE_TRUNCATED from mbedtls_test_message_queue_peek_info.
 */
int mbedtls_mock_tcp_recv_msg( void *ctx, unsigned char *buf, size_t buf_len )
{
    mbedtls_test_message_queue* queue;
    mbedtls_mock_socket* socket;
    mbedtls_test_message_socket_context *context = (mbedtls_test_message_socket_context*) ctx;
    size_t drop_len = 0;
    size_t msg_len;
    int ret;

    if( context == NULL || context->socket_ == NULL
        || context->queue_input == NULL )
    {
        return MBEDTLS_TEST_ERROR_CONTEXT_ERROR;
    }

    queue = context->queue_input;
    socket = context->socket_;

    /* Peek first, so that in case of a socket error the data remains in
     * the queue. */
    ret = mbedtls_test_message_queue_peek_info( queue, buf_len, &msg_len );
    if( ret == MBEDTLS_TEST_ERROR_MESSAGE_TRUNCATED )
    {
        /* Calculate how much to drop */
        drop_len = msg_len - buf_len;

        /* Set the requested message len to be buffer length */
        msg_len = buf_len;
    } else if( ret != 0 )
    {
        return ret;
    }

    if( mbedtls_mock_tcp_recv_b( socket, buf, msg_len ) != (int) msg_len )
        return MBEDTLS_TEST_ERROR_RECV_FAILED;

    if( ret == MBEDTLS_TEST_ERROR_MESSAGE_TRUNCATED )
    {
        /* Drop the remaining part of the message */
        if( mbedtls_mock_tcp_recv_b( socket, NULL, drop_len ) != (int) drop_len )
        {
          /* Inconsistent state - part of the message was read,
           * and a part couldn't. Not much we can do here, but it should not
           * happen in test environment, unless forced manually. */
        }
    }
    mbedtls_test_message_queue_pop_info( queue, buf_len );

    return msg_len;
}

#if defined(MBEDTLS_X509_CRT_PARSE_C) && \
    defined(MBEDTLS_ENTROPY_C) && \
    defined(MBEDTLS_CTR_DRBG_C)

/*
 * Structure with endpoint's certificates for SSL communication tests.
 */
typedef struct mbedtls_endpoint_certificate
{
    mbedtls_x509_crt ca_cert;
    mbedtls_x509_crt cert;
    mbedtls_pk_context pkey;
} mbedtls_endpoint_certificate;

/*
 * Endpoint structure for SSL communication tests.
 */
typedef struct mbedtls_endpoint
{
    const char *name;
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_entropy_context entropy;
    mbedtls_mock_socket socket_;
    mbedtls_endpoint_certificate cert;
} mbedtls_endpoint;

/*
 * Initializes \p ep_cert structure and assigns it to endpoint
 * represented by \p ep.
 *
 * \retval  0 on success, otherwise error code.
 */
int mbedtls_endpoint_certificate_init( mbedtls_endpoint *ep, int pk_alg )
{
    int i = 0;
    int ret = -1;
    mbedtls_endpoint_certificate *cert;

    if( ep == NULL )
    {
        return MBEDTLS_ERR_SSL_BAD_INPUT_DATA;
    }

    cert = &( ep->cert );
    mbedtls_x509_crt_init( &( cert->ca_cert ) );
    mbedtls_x509_crt_init( &( cert->cert ) );
    mbedtls_pk_init( &( cert->pkey ) );

    /* Load the trusted CA */

    for( i = 0; mbedtls_test_cas_der[i] != NULL; i++ )
    {
        ret = mbedtls_x509_crt_parse_der( &( cert->ca_cert ),
                        (const unsigned char *) mbedtls_test_cas_der[i],
                        mbedtls_test_cas_der_len[i] );
        TEST_ASSERT( ret == 0 );
    }

    /* Load own certificate and private key */

    if( ep->conf.endpoint == MBEDTLS_SSL_IS_SERVER )
    {
        if( pk_alg == MBEDTLS_PK_RSA )
        {
            ret = mbedtls_x509_crt_parse( &( cert->cert ),
                    (const unsigned char*) mbedtls_test_srv_crt_rsa_sha256_der,
                    mbedtls_test_srv_crt_rsa_sha256_der_len );
            TEST_ASSERT( ret == 0 );

            ret = mbedtls_pk_parse_key( &( cert->pkey ),
                            (const unsigned char*) mbedtls_test_srv_key_rsa_der,
                            mbedtls_test_srv_key_rsa_der_len, NULL, 0 );
            TEST_ASSERT( ret == 0 );
        }
        else
        {
            ret = mbedtls_x509_crt_parse( &( cert->cert ),
                            (const unsigned char*) mbedtls_test_srv_crt_ec_der,
                            mbedtls_test_srv_crt_ec_der_len );
            TEST_ASSERT( ret == 0 );

            ret = mbedtls_pk_parse_key( &( cert->pkey ),
                            (const unsigned char*) mbedtls_test_srv_key_ec_der,
                            mbedtls_test_srv_key_ec_der_len, NULL, 0 );
            TEST_ASSERT( ret == 0 );
        }
    }
    else
    {
        if( pk_alg == MBEDTLS_PK_RSA )
        {
            ret = mbedtls_x509_crt_parse( &( cert->cert ),
                          (const unsigned char *) mbedtls_test_cli_crt_rsa_der,
                          mbedtls_test_cli_crt_rsa_der_len );
            TEST_ASSERT( ret == 0 );

            ret = mbedtls_pk_parse_key( &( cert->pkey ),
                          (const unsigned char *) mbedtls_test_cli_key_rsa_der,
                          mbedtls_test_cli_key_rsa_der_len, NULL, 0 );
            TEST_ASSERT( ret == 0 );
        }
        else
        {
            ret = mbedtls_x509_crt_parse( &( cert->cert ),
                          (const unsigned char *) mbedtls_test_cli_crt_ec_der,
                          mbedtls_test_cli_crt_ec_len );
            TEST_ASSERT( ret == 0 );

            ret = mbedtls_pk_parse_key( &( cert->pkey ),
                          (const unsigned char *) mbedtls_test_cli_key_ec_der,
                          mbedtls_test_cli_key_ec_der_len, NULL, 0 );
            TEST_ASSERT( ret == 0 );
        }
    }

    mbedtls_ssl_conf_ca_chain( &( ep->conf ), &( cert->ca_cert ), NULL );

    ret = mbedtls_ssl_conf_own_cert( &( ep->conf ), &( cert->cert ),
                                     &( cert->pkey ) );
    TEST_ASSERT( ret == 0 );

exit:
    if( ret != 0 )
    {
        mbedtls_x509_crt_free( &( cert->ca_cert ) );
        mbedtls_x509_crt_free( &( cert->cert ) );
        mbedtls_pk_free( &( cert->pkey ) );
    }

    return ret;
}

/*
 * Initializes \p ep structure. It is important to call `mbedtls_endpoint_free()`
 * after calling this function even if it fails.
 *
 * \p endpoint_type must be set as MBEDTLS_SSL_IS_SERVER or
 * MBEDTLS_SSL_IS_CLIENT.
 * \p pk_alg the algorithm to use, currently only MBEDTLS_PK_RSA and
 * MBEDTLS_PK_ECDSA are supported.
 * \p dtls_context - in case of DTLS - this is the context handling metadata.
 * \p input_queue - used only in case of DTLS.
 * \p output_queue - used only in case of DTLS.
 *
 * \retval  0 on success, otherwise error code.
 */
int mbedtls_endpoint_init( mbedtls_endpoint *ep, int endpoint_type, int pk_alg,
                           mbedtls_test_message_socket_context *dtls_context,
                           mbedtls_test_message_queue *input_queue,
                           mbedtls_test_message_queue *output_queue )
{
    int ret = -1;

    if( dtls_context != NULL && ( input_queue == NULL || output_queue == NULL ) )
        return MBEDTLS_ERR_SSL_BAD_INPUT_DATA;

    if( ep == NULL )
        return MBEDTLS_ERR_SSL_BAD_INPUT_DATA;

    memset( ep, 0, sizeof( *ep ) );

    ep->name = ( endpoint_type == MBEDTLS_SSL_IS_SERVER ) ? "Server" : "Client";

    mbedtls_ssl_init( &( ep->ssl ) );
    mbedtls_ssl_config_init( &( ep->conf ) );
    mbedtls_ctr_drbg_init( &( ep->ctr_drbg ) );
    mbedtls_ssl_conf_rng( &( ep->conf ),
        mbedtls_ctr_drbg_random,
        &( ep->ctr_drbg ) );
    mbedtls_entropy_init( &( ep->entropy ) );
    if( dtls_context != NULL )
    {
        TEST_ASSERT( mbedtls_message_socket_setup( input_queue, output_queue,
                                                   100, &( ep->socket_ ),
                                                   dtls_context ) == 0 );
    }
    else
    {
        mbedtls_mock_socket_init( &( ep->socket_ ) );
    }

    ret = mbedtls_ctr_drbg_seed( &( ep->ctr_drbg ), mbedtls_entropy_func,
                    &( ep->entropy ), (const unsigned char *) ( ep->name ),
                    strlen( ep->name ) );
    TEST_ASSERT( ret == 0 );

    /* Non-blocking callbacks without timeout */
    if( dtls_context != NULL )
    {
        mbedtls_ssl_set_bio( &( ep->ssl ), dtls_context,
            mbedtls_mock_tcp_send_msg,
            mbedtls_mock_tcp_recv_msg,
            NULL );
    }
    else
    {
        mbedtls_ssl_set_bio( &( ep->ssl ), &( ep->socket_ ),
            mbedtls_mock_tcp_send_nb,
            mbedtls_mock_tcp_recv_nb,
            NULL );
    }

    ret = mbedtls_ssl_config_defaults( &( ep->conf ), endpoint_type,
                                       ( dtls_context != NULL ) ?
                                           MBEDTLS_SSL_TRANSPORT_DATAGRAM :
                                           MBEDTLS_SSL_TRANSPORT_STREAM,
                                       MBEDTLS_SSL_PRESET_DEFAULT );
    TEST_ASSERT( ret == 0 );

    ret = mbedtls_ssl_setup( &( ep->ssl ), &( ep->conf ) );
    TEST_ASSERT( ret == 0 );

#if defined(MBEDTLS_SSL_PROTO_DTLS) && defined(MBEDTLS_SSL_SRV_C)
    if( endpoint_type == MBEDTLS_SSL_IS_SERVER && dtls_context != NULL )
         mbedtls_ssl_conf_dtls_cookies( &( ep->conf ), NULL, NULL, NULL );
#endif

    ret = mbedtls_endpoint_certificate_init( ep, pk_alg );
    TEST_ASSERT( ret == 0 );

exit:
    return ret;
}

/*
 * Deinitializes certificates from endpoint represented by \p ep.
 */
void mbedtls_endpoint_certificate_free( mbedtls_endpoint *ep )
{
    mbedtls_endpoint_certificate *cert = &( ep->cert );
    mbedtls_x509_crt_free( &( cert->ca_cert ) );
    mbedtls_x509_crt_free( &( cert->cert ) );
    mbedtls_pk_free( &( cert->pkey ) );
}

/*
 * Deinitializes endpoint represented by \p ep.
 */
void mbedtls_endpoint_free( mbedtls_endpoint *ep,
                            mbedtls_test_message_socket_context *context )
{
    mbedtls_endpoint_certificate_free( ep );

    mbedtls_ssl_free( &( ep->ssl ) );
    mbedtls_ssl_config_free( &( ep->conf ) );
    mbedtls_ctr_drbg_free( &( ep->ctr_drbg ) );
    mbedtls_entropy_free( &( ep->entropy ) );

    if( context != NULL )
    {
        mbedtls_message_socket_close( context );
    }
    else
    {
        mbedtls_mock_socket_close( &( ep->socket_ ) );
    }
}

/*
 * This function moves ssl handshake from \p ssl to prescribed \p state.
 * /p second_ssl is used as second endpoint and their sockets have to be
 * connected before calling this function.
 *
 * \retval  0 on success, otherwise error code.
 */
int mbedtls_move_handshake_to_state( mbedtls_ssl_context *ssl,
                                     mbedtls_ssl_context *second_ssl,
                                     int state )
{
    enum { BUFFSIZE = 1024 };
    int max_steps = 1000;
    int ret = 0;
 
    if( ssl == NULL || second_ssl == NULL )
    {
        return MBEDTLS_ERR_SSL_BAD_INPUT_DATA;
    }

    /* Perform communication via connected sockets */
    while( ( ssl->state != state ) && ( --max_steps >= 0 ) )
    {
        /* If /p second_ssl ends the handshake procedure before /p ssl then
         * there is no need to call the next step */
        if( second_ssl->state != MBEDTLS_SSL_HANDSHAKE_OVER )
        {
            ret = mbedtls_ssl_handshake_step( second_ssl );
            if( ret != 0 && ret != MBEDTLS_ERR_SSL_WANT_READ &&
                            ret != MBEDTLS_ERR_SSL_WANT_WRITE )
            {
                return ret;
            }
        }

        /* We only care about the \p ssl state and returns, so we call it last,
         * to leave the iteration as soon as the state is as expected. */
        ret = mbedtls_ssl_handshake_step( ssl );
        if( ret != 0 && ret != MBEDTLS_ERR_SSL_WANT_READ &&
                        ret != MBEDTLS_ERR_SSL_WANT_WRITE )
        {
            return ret;
        }
    }

    return ( max_steps >= 0 ) ? ret : -1;
}

#endif /* MBEDTLS_X509_CRT_PARSE_C && MBEDTLS_ENTROPY_C && MBEDTLS_CTR_DRBG_C */

/*
 * Write application data. Increase write counter if necessary.
 */
int mbedtls_ssl_write_fragment( mbedtls_ssl_context *ssl, unsigned char *buf,
                                int buf_len, int *written,
                                const int expected_fragments )
{
    int ret = mbedtls_ssl_write( ssl, buf + *written, buf_len - *written );
    if( ret > 0 )
    {
        *written += ret;
    }

    if( expected_fragments == 0 )
    {
        /* Used for DTLS and the message size larger than MFL. In that case
         * the message can not be fragmented and the library should return
         * MBEDTLS_ERR_SSL_BAD_INPUT_DATA error. This error must be returned
         * to prevent a dead loop inside mbedtls_exchange_data(). */
        return ret;
    }
    else if( expected_fragments == 1 )
    {
        /* Used for TLS/DTLS and the message size lower than MFL */
        TEST_ASSERT( ret == buf_len ||
                     ret == MBEDTLS_ERR_SSL_WANT_READ ||
                     ret == MBEDTLS_ERR_SSL_WANT_WRITE );
    }
    else
    {
        /* Used for TLS and the message size larger than MFL */
        TEST_ASSERT( expected_fragments > 1 );
        TEST_ASSERT( ( ret >= 0 && ret <= buf_len ) ||
                       ret == MBEDTLS_ERR_SSL_WANT_READ ||
                       ret == MBEDTLS_ERR_SSL_WANT_WRITE );
    }

    return 0;

exit:
    /* Some of the tests failed */
    return -1;
}

/*
 * Read application data and increase read counter and fragments counter if necessary.
 */
int mbedtls_ssl_read_fragment( mbedtls_ssl_context *ssl, unsigned char *buf,
                               int buf_len, int *read,
                               int *fragments, const int expected_fragments )
{
    int ret = mbedtls_ssl_read( ssl, buf + *read, buf_len - *read );
    if( ret > 0 )
    {
        ( *fragments )++;
        *read += ret;
    }

    if( expected_fragments == 0 )
    {
        TEST_ASSERT( ret == 0 );
    }
    else if( expected_fragments == 1 )
    {
        TEST_ASSERT( ret == buf_len ||
                     ret == MBEDTLS_ERR_SSL_WANT_READ ||
                     ret == MBEDTLS_ERR_SSL_WANT_WRITE );
    }
    else
    {
        TEST_ASSERT( expected_fragments > 1 );
        TEST_ASSERT( ( ret >= 0 && ret <= buf_len ) ||
                       ret == MBEDTLS_ERR_SSL_WANT_READ ||
                       ret == MBEDTLS_ERR_SSL_WANT_WRITE );
    }

    return 0;

exit:
    /* Some of the tests failed */
    return -1;
}

/*
 * Helper function setting up inverse record transformations
 * using given cipher, hash, EtM mode, authentication tag length,
 * and version.
 */

#define CHK( x )                                \
    do                                          \
    {                                           \
        if( !( x ) )                            \
        {                                       \
            ret = -1;                           \
            goto cleanup;                       \
        }                                       \
    } while( 0 )

void set_ciphersuite( mbedtls_ssl_config *conf, const char *cipher,
                      uint16_t* forced_ciphersuite )
{
    const mbedtls_ssl_ciphersuite_t *ciphersuite_info;
    forced_ciphersuite[0] = mbedtls_ssl_get_ciphersuite_id( cipher );
    forced_ciphersuite[1] = 0;

    ciphersuite_info =
        mbedtls_ssl_ciphersuite_from_id( forced_ciphersuite[0] );

    TEST_ASSERT( ciphersuite_info != NULL );
    TEST_ASSERT( ciphersuite_info->min_minor_ver <= conf->max_minor_ver );
    TEST_ASSERT( ciphersuite_info->max_minor_ver >= conf->min_minor_ver );

    if( conf->max_minor_ver > ciphersuite_info->max_minor_ver )
    {
        conf->max_minor_ver = ciphersuite_info->max_minor_ver;
    }
    if( conf->min_minor_ver < ciphersuite_info->min_minor_ver )
    {
        conf->min_minor_ver = ciphersuite_info->min_minor_ver;
    }

    mbedtls_ssl_conf_ciphersuites( conf, forced_ciphersuite );

exit:
    return;
}

int psk_dummy_callback( void *p_info, mbedtls_ssl_context *ssl,
                  const unsigned char *name, size_t name_len )
{
    (void) p_info;
    (void) ssl;
    (void) name;
    (void) name_len;

    return ( 0 );
}

#if MBEDTLS_SSL_CID_OUT_LEN_MAX > MBEDTLS_SSL_CID_IN_LEN_MAX
#define SSL_CID_LEN_MIN MBEDTLS_SSL_CID_IN_LEN_MAX
#else
#define SSL_CID_LEN_MIN MBEDTLS_SSL_CID_OUT_LEN_MAX
#endif

static int build_transforms( mbedtls_ssl_transform *t_in,
                             mbedtls_ssl_transform *t_out,
                             int cipher_type, int hash_id,
                             int etm, int tag_mode, int ver,
                             size_t cid0_len,
                             size_t cid1_len )
{
    mbedtls_cipher_info_t const *cipher_info;
    int ret = 0;

    size_t keylen, maclen, ivlen;
    unsigned char *key0 = NULL, *key1 = NULL;
    unsigned char *md0 = NULL, *md1 = NULL;
    unsigned char iv_enc[16], iv_dec[16];

#if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
    unsigned char cid0[ SSL_CID_LEN_MIN ];
    unsigned char cid1[ SSL_CID_LEN_MIN ];

    mbedtls_test_rnd_std_rand( NULL, cid0, sizeof( cid0 ) );
    mbedtls_test_rnd_std_rand( NULL, cid1, sizeof( cid1 ) );
#else
    ((void) cid0_len);
    ((void) cid1_len);
#endif /* MBEDTLS_SSL_DTLS_CONNECTION_ID */

    maclen = 0;

    /* Pick cipher */
    cipher_info = mbedtls_cipher_info_from_type( cipher_type );
    CHK( cipher_info != NULL );
    CHK( cipher_info->iv_size <= 16 );
    CHK( cipher_info->key_bitlen % 8 == 0 );

    /* Pick keys */
    keylen = cipher_info->key_bitlen / 8;
    /* Allocate `keylen + 1` bytes to ensure that we get
     * a non-NULL pointers from `mbedtls_calloc` even if
     * `keylen == 0` in the case of the NULL cipher. */
    CHK( ( key0 = mbedtls_calloc( 1, keylen + 1 ) ) != NULL );
    CHK( ( key1 = mbedtls_calloc( 1, keylen + 1 ) ) != NULL );
    memset( key0, 0x1, keylen );
    memset( key1, 0x2, keylen );

    /* Setup cipher contexts */
    CHK( mbedtls_cipher_setup( &t_in->cipher_ctx_enc,  cipher_info ) == 0 );
    CHK( mbedtls_cipher_setup( &t_in->cipher_ctx_dec,  cipher_info ) == 0 );
    CHK( mbedtls_cipher_setup( &t_out->cipher_ctx_enc, cipher_info ) == 0 );
    CHK( mbedtls_cipher_setup( &t_out->cipher_ctx_dec, cipher_info ) == 0 );

#if defined(MBEDTLS_CIPHER_MODE_CBC)
    if( cipher_info->mode == MBEDTLS_MODE_CBC )
    {
        CHK( mbedtls_cipher_set_padding_mode( &t_in->cipher_ctx_enc,
                                              MBEDTLS_PADDING_NONE ) == 0 );
        CHK( mbedtls_cipher_set_padding_mode( &t_in->cipher_ctx_dec,
                                              MBEDTLS_PADDING_NONE ) == 0 );
        CHK( mbedtls_cipher_set_padding_mode( &t_out->cipher_ctx_enc,
                                              MBEDTLS_PADDING_NONE ) == 0 );
        CHK( mbedtls_cipher_set_padding_mode( &t_out->cipher_ctx_dec,
                                              MBEDTLS_PADDING_NONE ) == 0 );
    }
#endif /* MBEDTLS_CIPHER_MODE_CBC */

    CHK( mbedtls_cipher_setkey( &t_in->cipher_ctx_enc, key0,
                                keylen << 3, MBEDTLS_ENCRYPT ) == 0 );
    CHK( mbedtls_cipher_setkey( &t_in->cipher_ctx_dec, key1,
                                keylen << 3, MBEDTLS_DECRYPT ) == 0 );
    CHK( mbedtls_cipher_setkey( &t_out->cipher_ctx_enc, key1,
                                keylen << 3, MBEDTLS_ENCRYPT ) == 0 );
    CHK( mbedtls_cipher_setkey( &t_out->cipher_ctx_dec, key0,
                                keylen << 3, MBEDTLS_DECRYPT ) == 0 );

    /* Setup MAC contexts */
#if defined(MBEDTLS_SSL_SOME_MODES_USE_MAC)
    if( cipher_info->mode == MBEDTLS_MODE_CBC ||
        cipher_info->mode == MBEDTLS_MODE_STREAM )
    {
        mbedtls_md_info_t const *md_info;

        /* Pick hash */
        md_info = mbedtls_md_info_from_type( hash_id );
        CHK( md_info != NULL );

        /* Pick hash keys */
        maclen = mbedtls_md_get_size( md_info );
        CHK( ( md0 = mbedtls_calloc( 1, maclen ) ) != NULL );
        CHK( ( md1 = mbedtls_calloc( 1, maclen ) ) != NULL );
        memset( md0, 0x5, maclen );
        memset( md1, 0x6, maclen );

        CHK( mbedtls_md_setup( &t_out->md_ctx_enc, md_info, 1 ) == 0 );
        CHK( mbedtls_md_setup( &t_out->md_ctx_dec, md_info, 1 ) == 0 );
        CHK( mbedtls_md_setup( &t_in->md_ctx_enc,  md_info, 1 ) == 0 );
        CHK( mbedtls_md_setup( &t_in->md_ctx_dec,  md_info, 1 ) == 0 );

        if( ver > MBEDTLS_SSL_MINOR_VERSION_0 )
        {
            CHK( mbedtls_md_hmac_starts( &t_in->md_ctx_enc,
                                         md0, maclen ) == 0 );
            CHK( mbedtls_md_hmac_starts( &t_in->md_ctx_dec,
                                         md1, maclen ) == 0 );
            CHK( mbedtls_md_hmac_starts( &t_out->md_ctx_enc,
                                         md1, maclen ) == 0 );
            CHK( mbedtls_md_hmac_starts( &t_out->md_ctx_dec,
                                         md0, maclen ) == 0 );
        }
#if defined(MBEDTLS_SSL_PROTO_SSL3)
        else
        {
            memcpy( &t_in->mac_enc, md0, maclen );
            memcpy( &t_in->mac_dec, md1, maclen );
            memcpy( &t_out->mac_enc, md1, maclen );
            memcpy( &t_out->mac_dec, md0, maclen );
        }
#endif
    }
#else
    ((void) hash_id);
#endif /* MBEDTLS_SSL_SOME_MODES_USE_MAC */


    /* Pick IV's (regardless of whether they
     * are being used by the transform). */
    ivlen = cipher_info->iv_size;
    memset( iv_enc, 0x3, sizeof( iv_enc ) );
    memset( iv_dec, 0x4, sizeof( iv_dec ) );

    /*
     * Setup transforms
     */

#if defined(MBEDTLS_SSL_ENCRYPT_THEN_MAC) && \
    defined(MBEDTLS_SSL_SOME_MODES_USE_MAC)
    t_out->encrypt_then_mac = etm;
    t_in->encrypt_then_mac = etm;
#else
    ((void) etm);
#endif

    t_out->minor_ver = ver;
    t_in->minor_ver = ver;
    t_out->ivlen = ivlen;
    t_in->ivlen = ivlen;

    switch( cipher_info->mode )
    {
        case MBEDTLS_MODE_GCM:
        case MBEDTLS_MODE_CCM:
#if defined(MBEDTLS_SSL_PROTO_TLS1_3_EXPERIMENTAL)
            if( ver == MBEDTLS_SSL_MINOR_VERSION_4 )
            {
                t_out->fixed_ivlen = 12;
                t_in->fixed_ivlen  = 12;
            }
            else
#endif /* MBEDTLS_SSL_PROTO_TLS1_3_EXPERIMENTAL */
            {
                t_out->fixed_ivlen = 4;
                t_in->fixed_ivlen = 4;
            }
            t_out->maclen = 0;
            t_in->maclen = 0;
            switch( tag_mode )
            {
                case 0: /* Full tag */
                    t_out->taglen = 16;
                    t_in->taglen = 16;
                    break;
                case 1: /* Partial tag */
                    t_out->taglen = 8;
                    t_in->taglen = 8;
                    break;
                default:
                    ret = 1;
                    goto cleanup;
            }
            break;

        case MBEDTLS_MODE_CHACHAPOLY:
            t_out->fixed_ivlen = 12;
            t_in->fixed_ivlen = 12;
            t_out->maclen = 0;
            t_in->maclen = 0;
            switch( tag_mode )
            {
                case 0: /* Full tag */
                    t_out->taglen = 16;
                    t_in->taglen = 16;
                    break;
                case 1: /* Partial tag */
                    t_out->taglen = 8;
                    t_in->taglen = 8;
                    break;
                default:
                    ret = 1;
                    goto cleanup;
            }
            break;

        case MBEDTLS_MODE_STREAM:
        case MBEDTLS_MODE_CBC:
            t_out->fixed_ivlen = 0; /* redundant, must be 0 */
            t_in->fixed_ivlen = 0;  /* redundant, must be 0 */
            t_out->taglen = 0;
            t_in->taglen = 0;
            switch( tag_mode )
            {
                case 0: /* Full tag */
                    t_out->maclen = maclen;
                    t_in->maclen = maclen;
                    break;
                case 1: /* Partial tag */
                    t_out->maclen = 10;
                    t_in->maclen = 10;
                    break;
                default:
                    ret = 1;
                    goto cleanup;
            }
            break;
        default:
            ret = 1;
            goto cleanup;
            break;
    }

    /* Setup IV's */

    memcpy( &t_in->iv_dec, iv_dec, sizeof( iv_dec ) );
    memcpy( &t_in->iv_enc, iv_enc, sizeof( iv_enc ) );
    memcpy( &t_out->iv_dec, iv_enc, sizeof( iv_enc ) );
    memcpy( &t_out->iv_enc, iv_dec, sizeof( iv_dec ) );

#if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
    /* Add CID */
    memcpy( &t_in->in_cid,  cid0, cid0_len );
    memcpy( &t_in->out_cid, cid1, cid1_len );
    t_in->in_cid_len = cid0_len;
    t_in->out_cid_len = cid1_len;
    memcpy( &t_out->in_cid,  cid1, cid1_len );
    memcpy( &t_out->out_cid, cid0, cid0_len );
    t_out->in_cid_len = cid1_len;
    t_out->out_cid_len = cid0_len;
#endif /* MBEDTLS_SSL_DTLS_CONNECTION_ID */

cleanup:

    mbedtls_free( key0 );
    mbedtls_free( key1 );

    mbedtls_free( md0 );
    mbedtls_free( md1 );

    return( ret );
}

/*
 * Populate a session structure for serialization tests.
 * Choose dummy values, mostly non-0 to distinguish from the init default.
 */
static int ssl_populate_session( mbedtls_ssl_session *session,
                                 int ticket_len,
                                 const char *crt_file )
{
#if defined(MBEDTLS_HAVE_TIME)
    session->start = mbedtls_time( NULL ) - 42;
#endif
    session->ciphersuite = 0xabcd;
    session->compression = 1;
    session->id_len = sizeof( session->id );
    memset( session->id, 66, session->id_len );
    memset( session->master, 17, sizeof( session->master ) );

#if defined(MBEDTLS_X509_CRT_PARSE_C) && defined(MBEDTLS_FS_IO)
    if( strlen( crt_file ) != 0 )
    {
        mbedtls_x509_crt tmp_crt;
        int ret;

        mbedtls_x509_crt_init( &tmp_crt );
        ret = mbedtls_x509_crt_parse_file( &tmp_crt, crt_file );
        if( ret != 0 )
            return( ret );

#if defined(MBEDTLS_SSL_KEEP_PEER_CERTIFICATE)
        /* Move temporary CRT. */
        session->peer_cert = mbedtls_calloc( 1, sizeof( *session->peer_cert ) );
        if( session->peer_cert == NULL )
            return( -1 );
        *session->peer_cert = tmp_crt;
        memset( &tmp_crt, 0, sizeof( tmp_crt ) );
#else /* MBEDTLS_SSL_KEEP_PEER_CERTIFICATE */
        /* Calculate digest of temporary CRT. */
        session->peer_cert_digest =
            mbedtls_calloc( 1, MBEDTLS_SSL_PEER_CERT_DIGEST_DFL_LEN );
        if( session->peer_cert_digest == NULL )
            return( -1 );
        ret = mbedtls_md( mbedtls_md_info_from_type(
                              MBEDTLS_SSL_PEER_CERT_DIGEST_DFL_TYPE ),
                          tmp_crt.raw.p, tmp_crt.raw.len,
                          session->peer_cert_digest );
        if( ret != 0 )
            return( ret );
        session->peer_cert_digest_type =
            MBEDTLS_SSL_PEER_CERT_DIGEST_DFL_TYPE;
        session->peer_cert_digest_len =
            MBEDTLS_SSL_PEER_CERT_DIGEST_DFL_LEN;
#endif /* MBEDTLS_SSL_KEEP_PEER_CERTIFICATE */

        mbedtls_x509_crt_free( &tmp_crt );
    }
#else /* MBEDTLS_X509_CRT_PARSE_C && MBEDTLS_FS_IO */
    (void) crt_file;
#endif /* MBEDTLS_X509_CRT_PARSE_C && MBEDTLS_FS_IO */
    session->verify_result = 0xdeadbeef;

#if defined(MBEDTLS_SSL_SESSION_TICKETS) && defined(MBEDTLS_SSL_CLI_C)
    if( ticket_len != 0 )
    {
        session->ticket = mbedtls_calloc( 1, ticket_len );
        if( session->ticket == NULL )
            return( -1 );
        memset( session->ticket, 33, ticket_len );
    }
    session->ticket_len = ticket_len;
    session->ticket_lifetime = 86401;
#else
    (void) ticket_len;
#endif

#if defined(MBEDTLS_SSL_MAX_FRAGMENT_LENGTH)
    session->mfl_code = 1;
#endif
#if defined(MBEDTLS_SSL_ENCRYPT_THEN_MAC)
    session->encrypt_then_mac = 1;
#endif

    return( 0 );
}

/*
 * Perform data exchanging between \p ssl_1 and \p ssl_2 and check if the
 * message was sent in the correct number of fragments.
 *
 * /p ssl_1 and /p ssl_2    Endpoints represented by mbedtls_ssl_context. Both
 *                          of them must be initialized and connected beforehand.
 * /p msg_len_1 and /p msg_len_2 specify the size of the message to send.
 * /p expected_fragments_1 and /p expected_fragments_2 determine in how many
 *                          fragments the message should be sent.
 *      expected_fragments is 0: can be used for DTLS testing while the message
 *                          size is larger than MFL. In that case the message
 *                          cannot be fragmented and sent to the second endpoint.
 *                          This value can be used for negative tests.
 *      expected_fragments is 1: can be used for TLS/DTLS testing while the
 *                          message size is below MFL
 *      expected_fragments > 1: can be used for TLS testing while the message
 *                          size is larger than MFL
 *
 * \retval  0 on success, otherwise error code.
 */
int mbedtls_exchange_data( mbedtls_ssl_context *ssl_1,
                           int msg_len_1, const int expected_fragments_1,
                           mbedtls_ssl_context *ssl_2,
                           int msg_len_2, const int expected_fragments_2 )
{
    unsigned char *msg_buf_1 = malloc( msg_len_1 );
    unsigned char *msg_buf_2 = malloc( msg_len_2 );
    unsigned char *in_buf_1  = malloc( msg_len_2 );
    unsigned char *in_buf_2  = malloc( msg_len_1 );
    int msg_type, ret = -1;

    /* Perform this test with two message types. At first use a message
     * consisting of only 0x00 for the client and only 0xFF for the server.
     * At the second time use message with generated data */
    for( msg_type = 0; msg_type < 2; msg_type++ )
    {
        int written_1 = 0;
        int written_2 = 0;
        int read_1 = 0;
        int read_2 = 0;
        int fragments_1 = 0;
        int fragments_2 = 0;

        if( msg_type == 0 )
        {
            memset( msg_buf_1, 0x00, msg_len_1 );
            memset( msg_buf_2, 0xff, msg_len_2 );
        }
        else
        {
            int i, j = 0;
            for( i = 0; i < msg_len_1; i++ )
            {
                msg_buf_1[i] = j++ & 0xFF;
            }
            for( i = 0; i < msg_len_2; i++ )
            {
                msg_buf_2[i] = ( j -= 5 ) & 0xFF;
            }
        }

        while( read_1 < msg_len_2 || read_2 < msg_len_1 )
        {
            /* ssl_1 sending */
            if( msg_len_1 > written_1 )
            {
                ret = mbedtls_ssl_write_fragment( ssl_1, msg_buf_1,
                                                  msg_len_1, &written_1,
                                                  expected_fragments_1 );
                if( expected_fragments_1 == 0 )
                {
                    /* This error is expected when the message is too large and
                     * cannot be fragmented */
                    TEST_ASSERT( ret == MBEDTLS_ERR_SSL_BAD_INPUT_DATA );
                    msg_len_1 = 0;
                }
                else
                {
                    TEST_ASSERT( ret == 0 );
                }
            }

            /* ssl_2 sending */
            if( msg_len_2 > written_2 )
            {
                ret = mbedtls_ssl_write_fragment( ssl_2, msg_buf_2,
                                                  msg_len_2, &written_2,
                                                  expected_fragments_2 );
                if( expected_fragments_2 == 0 )
                {
                    /* This error is expected when the message is too large and
                     * cannot be fragmented */
                    TEST_ASSERT( ret == MBEDTLS_ERR_SSL_BAD_INPUT_DATA );
                    msg_len_2 = 0;
                }
                else
                {
                    TEST_ASSERT( ret == 0 );
                }
            }

            /* ssl_1 reading */
            if( read_1 < msg_len_2 )
            {
                ret = mbedtls_ssl_read_fragment( ssl_1, in_buf_1,
                                                 msg_len_2, &read_1,
                                                 &fragments_2,
                                                 expected_fragments_2 );
                TEST_ASSERT( ret == 0 );
            }

            /* ssl_2 reading */
            if( read_2 < msg_len_1 )
            {
                ret = mbedtls_ssl_read_fragment( ssl_2, in_buf_2,
                                                 msg_len_1, &read_2,
                                                 &fragments_1,
                                                 expected_fragments_1 );
                TEST_ASSERT( ret == 0 );
            }
        }

        ret = -1;
        TEST_ASSERT( 0 == memcmp( msg_buf_1, in_buf_2, msg_len_1 ) );
        TEST_ASSERT( 0 == memcmp( msg_buf_2, in_buf_1, msg_len_2 ) );
        TEST_ASSERT( fragments_1 == expected_fragments_1 );
        TEST_ASSERT( fragments_2 == expected_fragments_2 );
    }

    ret = 0;

exit:
    free( msg_buf_1 );
    free( in_buf_1 );
    free( msg_buf_2 );
    free( in_buf_2 );

    return ret;
}

/*
 * Perform data exchanging between \p ssl_1 and \p ssl_2. Both of endpoints
 * must be initialized and connected beforehand.
 *
 * \retval  0 on success, otherwise error code.
 */
int exchange_data( mbedtls_ssl_context *ssl_1,
                   mbedtls_ssl_context *ssl_2 )
{
    return mbedtls_exchange_data( ssl_1, 256, 1,
                                  ssl_2, 256, 1 );
}

#if defined(MBEDTLS_X509_CRT_PARSE_C) && \
    defined(MBEDTLS_ENTROPY_C) && \
    defined(MBEDTLS_CTR_DRBG_C)
void perform_handshake( handshake_test_options* options )
{
    /* forced_ciphersuite needs to last until the end of the handshake */
    uint16_t forced_ciphersuite[2];
    enum { BUFFSIZE = 17000 };
    mbedtls_endpoint client, server;
#if defined(MBEDTLS_KEY_EXCHANGE_SOME_PSK_ENABLED)
    const char *psk_identity = "foo";
#endif
#if defined(MBEDTLS_TIMING_C)
    mbedtls_timing_delay_context timer_client, timer_server;
#endif
#if defined(MBEDTLS_SSL_CONTEXT_SERIALIZATION)
    unsigned char *context_buf = NULL;
    size_t context_buf_len;
#endif
#if defined(MBEDTLS_SSL_RENEGOTIATION)
    int ret = -1;
#endif
    int expected_handshake_result = 0;

    mbedtls_test_message_queue server_queue, client_queue;
    mbedtls_test_message_socket_context server_context, client_context;
    mbedtls_message_socket_init( &server_context );
    mbedtls_message_socket_init( &client_context );

    /* Client side */
    if( options->dtls != 0 )
    {
        TEST_ASSERT( mbedtls_endpoint_init( &client, MBEDTLS_SSL_IS_CLIENT,
                                            options->pk_alg, &client_context,
                                            &client_queue,
                                            &server_queue ) == 0 );
#if defined(MBEDTLS_TIMING_C)
        mbedtls_ssl_set_timer_cb( &client.ssl, &timer_client,
                                  mbedtls_timing_set_delay,
                                  mbedtls_timing_get_delay );
#endif
    }
    else
    {
        TEST_ASSERT( mbedtls_endpoint_init( &client, MBEDTLS_SSL_IS_CLIENT,
                                            options->pk_alg, NULL, NULL,
                                            NULL ) == 0 );
    }

    if( options->client_min_version != TEST_SSL_MINOR_VERSION_NONE )
    {
        mbedtls_ssl_conf_min_version( &client.conf, MBEDTLS_SSL_MAJOR_VERSION_3,
                                          options->client_min_version );
    }

    if( options->client_max_version != TEST_SSL_MINOR_VERSION_NONE )
    {
        mbedtls_ssl_conf_max_version( &client.conf, MBEDTLS_SSL_MAJOR_VERSION_3,
                                          options->client_max_version );
    }

    if( strlen( options->cipher ) > 0 )
    {
        set_ciphersuite( &client.conf, options->cipher, forced_ciphersuite );
    }

#if defined (MBEDTLS_DEBUG_C)
    if( options->cli_log_fun )
    {
        mbedtls_debug_set_threshold( 4 );
        mbedtls_ssl_conf_dbg( &client.conf, options->cli_log_fun,
                                            options->cli_log_obj );
    }
#endif

    /* Server side */
    if( options->dtls != 0 )
    {
        TEST_ASSERT( mbedtls_endpoint_init( &server, MBEDTLS_SSL_IS_SERVER,
                                            options->pk_alg, &server_context,
                                            &server_queue,
                                            &client_queue) == 0 );
#if defined(MBEDTLS_TIMING_C)
        mbedtls_ssl_set_timer_cb( &server.ssl, &timer_server,
                                  mbedtls_timing_set_delay,
                                  mbedtls_timing_get_delay );
#endif
    }
    else
    {
        TEST_ASSERT( mbedtls_endpoint_init( &server, MBEDTLS_SSL_IS_SERVER,
                                            options->pk_alg, NULL, NULL, NULL ) == 0 );
    }

    mbedtls_ssl_conf_authmode( &server.conf, options->srv_auth_mode );

    if( options->server_min_version != TEST_SSL_MINOR_VERSION_NONE )
    {
        mbedtls_ssl_conf_min_version( &server.conf, MBEDTLS_SSL_MAJOR_VERSION_3,
                                          options->server_min_version );
    }

    if( options->server_max_version != TEST_SSL_MINOR_VERSION_NONE )
    {
        mbedtls_ssl_conf_max_version( &server.conf, MBEDTLS_SSL_MAJOR_VERSION_3,
                                          options->server_max_version );
    }

#if defined(MBEDTLS_SSL_MAX_FRAGMENT_LENGTH)
    TEST_ASSERT( mbedtls_ssl_conf_max_frag_len( &(server.conf),
                                         (unsigned char) options->mfl ) == 0 );
    TEST_ASSERT( mbedtls_ssl_conf_max_frag_len( &(client.conf),
                                         (unsigned char) options->mfl ) == 0 );
#else
    TEST_ASSERT( MBEDTLS_SSL_MAX_FRAG_LEN_NONE == options->mfl );
#endif /* MBEDTLS_SSL_MAX_FRAGMENT_LENGTH */

#if defined(MBEDTLS_KEY_EXCHANGE_SOME_PSK_ENABLED)
    if( options->psk_str != NULL && options->psk_str->len > 0 )
    {
        TEST_ASSERT( mbedtls_ssl_conf_psk( &client.conf, options->psk_str->x,
                             options->psk_str->len,
                             (const unsigned char *) psk_identity,
                             strlen( psk_identity ) ) == 0 );

        TEST_ASSERT( mbedtls_ssl_conf_psk( &server.conf, options->psk_str->x,
                             options->psk_str->len,
                             (const unsigned char *) psk_identity,
                             strlen( psk_identity ) ) == 0 );

        mbedtls_ssl_conf_psk_cb( &server.conf, psk_dummy_callback, NULL );
    }
#endif
#if defined(MBEDTLS_SSL_RENEGOTIATION)
    if( options->renegotiate )
    {
        mbedtls_ssl_conf_renegotiation( &(server.conf),
                                        MBEDTLS_SSL_RENEGOTIATION_ENABLED );
        mbedtls_ssl_conf_renegotiation( &(client.conf),
                                        MBEDTLS_SSL_RENEGOTIATION_ENABLED );

        mbedtls_ssl_conf_legacy_renegotiation( &(server.conf),
                                               options->legacy_renegotiation );
        mbedtls_ssl_conf_legacy_renegotiation( &(client.conf),
                                               options->legacy_renegotiation );
    }
#endif /* MBEDTLS_SSL_RENEGOTIATION */

#if defined (MBEDTLS_DEBUG_C)
    if( options->srv_log_fun )
    {
        mbedtls_debug_set_threshold( 4 );
        mbedtls_ssl_conf_dbg( &server.conf, options->srv_log_fun,
                                            options->srv_log_obj );
    }
#endif

    TEST_ASSERT( mbedtls_mock_socket_connect( &(client.socket_),
                                              &(server.socket_),
                                              BUFFSIZE ) == 0 );

#if defined(MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH)
    if( options->resize_buffers != 0 )
    {
        /* Ensure that the buffer sizes are appropriate before resizes */
        TEST_ASSERT( client.ssl.out_buf_len == MBEDTLS_SSL_OUT_BUFFER_LEN );
        TEST_ASSERT( client.ssl.in_buf_len == MBEDTLS_SSL_IN_BUFFER_LEN );
        TEST_ASSERT( server.ssl.out_buf_len == MBEDTLS_SSL_OUT_BUFFER_LEN );
        TEST_ASSERT( server.ssl.in_buf_len == MBEDTLS_SSL_IN_BUFFER_LEN );
    }
#endif

    if( options->expected_negotiated_version == TEST_SSL_MINOR_VERSION_NONE )
    {
        expected_handshake_result = MBEDTLS_ERR_SSL_BAD_HS_PROTOCOL_VERSION;
    }

    TEST_ASSERT( mbedtls_move_handshake_to_state( &(client.ssl),
                                                  &(server.ssl),
                                                  MBEDTLS_SSL_HANDSHAKE_OVER )
                 ==  expected_handshake_result );

    if( expected_handshake_result != 0 )
    {
        /* Connection will have failed by this point, skip to cleanup */
        goto exit;
    }

    TEST_ASSERT( client.ssl.state == MBEDTLS_SSL_HANDSHAKE_OVER );
    TEST_ASSERT( server.ssl.state == MBEDTLS_SSL_HANDSHAKE_OVER );

    /* Check that we agree on the version... */
    TEST_ASSERT( client.ssl.minor_ver == server.ssl.minor_ver );

    /* And check that the version negotiated is the expected one. */
    TEST_EQUAL( client.ssl.minor_ver, options->expected_negotiated_version );

#if defined(MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH)
    if( options->resize_buffers != 0 )
    {
        if( options->expected_negotiated_version != MBEDTLS_SSL_MINOR_VERSION_0 &&
            options->expected_negotiated_version != MBEDTLS_SSL_MINOR_VERSION_1 )
        {
            /* A server, when using DTLS, might delay a buffer resize to happen
             * after it receives a message, so we force it. */
            TEST_ASSERT( exchange_data( &(client.ssl), &(server.ssl) ) == 0 );

            TEST_ASSERT( client.ssl.out_buf_len ==
                         mbedtls_ssl_get_output_buflen( &client.ssl ) );
            TEST_ASSERT( client.ssl.in_buf_len ==
                         mbedtls_ssl_get_input_buflen( &client.ssl ) );
            TEST_ASSERT( server.ssl.out_buf_len ==
                         mbedtls_ssl_get_output_buflen( &server.ssl ) );
            TEST_ASSERT( server.ssl.in_buf_len ==
                         mbedtls_ssl_get_input_buflen( &server.ssl ) );
        }
    }
#endif

    if( options->cli_msg_len != 0 || options->srv_msg_len != 0 )
    {
        /* Start data exchanging test */
        TEST_ASSERT( mbedtls_exchange_data( &(client.ssl), options->cli_msg_len,
                                            options->expected_cli_fragments,
                                            &(server.ssl), options->srv_msg_len,
                                            options->expected_srv_fragments )
                     == 0 );
    }
#if defined(MBEDTLS_SSL_CONTEXT_SERIALIZATION)
    if( options->serialize == 1 )
    {
        TEST_ASSERT( options->dtls == 1 );

        TEST_ASSERT( mbedtls_ssl_context_save( &(server.ssl), NULL,
                                               0, &context_buf_len )
                                             == MBEDTLS_ERR_SSL_BUFFER_TOO_SMALL );

        context_buf = mbedtls_calloc( 1, context_buf_len );
        TEST_ASSERT( context_buf != NULL );

        TEST_ASSERT( mbedtls_ssl_context_save( &(server.ssl), context_buf,
                                               context_buf_len,
                                               &context_buf_len ) == 0 );

        mbedtls_ssl_free( &(server.ssl) );
        mbedtls_ssl_init( &(server.ssl) );

        TEST_ASSERT( mbedtls_ssl_setup( &(server.ssl), &(server.conf) ) == 0 );

        mbedtls_ssl_set_bio( &( server.ssl ), &server_context,
                             mbedtls_mock_tcp_send_msg,
                             mbedtls_mock_tcp_recv_msg,
                             NULL );

#if defined(MBEDTLS_TIMING_C)
        mbedtls_ssl_set_timer_cb( &server.ssl, &timer_server,
                                  mbedtls_timing_set_delay,
                                  mbedtls_timing_get_delay );
#endif
#if defined(MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH)
        if( options->resize_buffers != 0 )
        {
            /* Ensure that the buffer sizes are appropriate before resizes */
            TEST_ASSERT( server.ssl.out_buf_len == MBEDTLS_SSL_OUT_BUFFER_LEN );
            TEST_ASSERT( server.ssl.in_buf_len == MBEDTLS_SSL_IN_BUFFER_LEN );
        }
#endif
        TEST_ASSERT( mbedtls_ssl_context_load( &( server.ssl ), context_buf,
                                               context_buf_len ) == 0 );

#if defined(MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH)
        /* Validate buffer sizes after context deserialization */
        if( options->resize_buffers != 0 )
        {
            TEST_ASSERT( server.ssl.out_buf_len ==
                         mbedtls_ssl_get_output_buflen( &server.ssl ) );
            TEST_ASSERT( server.ssl.in_buf_len ==
                         mbedtls_ssl_get_input_buflen( &server.ssl ) );
        }
#endif
        /* Retest writing/reading */
        if( options->cli_msg_len != 0 || options->srv_msg_len != 0 )
        {
            TEST_ASSERT( mbedtls_exchange_data( &(client.ssl),
                                                options->cli_msg_len,
                                                options->expected_cli_fragments,
                                                &(server.ssl),
                                                options->srv_msg_len,
                                                options->expected_srv_fragments )
                                                == 0 );
        }
    }
#endif /* MBEDTLS_SSL_CONTEXT_SERIALIZATION */

#if defined(MBEDTLS_SSL_RENEGOTIATION)
    if( options->renegotiate )
    {
        /* Start test with renegotiation */
        TEST_ASSERT( server.ssl.renego_status ==
                     MBEDTLS_SSL_INITIAL_HANDSHAKE );
        TEST_ASSERT( client.ssl.renego_status ==
                     MBEDTLS_SSL_INITIAL_HANDSHAKE );

        /* After calling this function for the server, it only sends a handshake
         * request. All renegotiation should happen during data exchanging */
        TEST_ASSERT( mbedtls_ssl_renegotiate( &(server.ssl) ) == 0 );
        TEST_ASSERT( server.ssl.renego_status ==
                     MBEDTLS_SSL_RENEGOTIATION_PENDING );
        TEST_ASSERT( client.ssl.renego_status ==
                     MBEDTLS_SSL_INITIAL_HANDSHAKE );

        TEST_ASSERT( exchange_data( &(client.ssl), &(server.ssl) ) == 0 );
        TEST_ASSERT( server.ssl.renego_status ==
                     MBEDTLS_SSL_RENEGOTIATION_DONE );
        TEST_ASSERT( client.ssl.renego_status ==
                     MBEDTLS_SSL_RENEGOTIATION_DONE );

        /* After calling mbedtls_ssl_renegotiate for the client all renegotiation
         * should happen inside this function. However in this test, we cannot
         * perform simultaneous communication betwen client and server so this
         * function will return waiting error on the socket. All rest of
         * renegotiation should happen during data exchanging */
        ret = mbedtls_ssl_renegotiate( &(client.ssl) );
#if defined(MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH)
        if( options->resize_buffers != 0 )
        {
            /* Ensure that the buffer sizes are appropriate before resizes */
            TEST_ASSERT( client.ssl.out_buf_len == MBEDTLS_SSL_OUT_BUFFER_LEN );
            TEST_ASSERT( client.ssl.in_buf_len == MBEDTLS_SSL_IN_BUFFER_LEN );
        }
#endif
        TEST_ASSERT( ret == 0 ||
                     ret == MBEDTLS_ERR_SSL_WANT_READ ||
                     ret == MBEDTLS_ERR_SSL_WANT_WRITE );
        TEST_ASSERT( server.ssl.renego_status ==
                     MBEDTLS_SSL_RENEGOTIATION_DONE );
        TEST_ASSERT( client.ssl.renego_status ==
                     MBEDTLS_SSL_RENEGOTIATION_IN_PROGRESS );

        TEST_ASSERT( exchange_data( &(client.ssl), &(server.ssl) ) == 0 );
        TEST_ASSERT( server.ssl.renego_status ==
                     MBEDTLS_SSL_RENEGOTIATION_DONE );
        TEST_ASSERT( client.ssl.renego_status ==
                     MBEDTLS_SSL_RENEGOTIATION_DONE );
#if defined(MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH)
        /* Validate buffer sizes after renegotiation */
        if( options->resize_buffers != 0 )
        {
            TEST_ASSERT( client.ssl.out_buf_len ==
                         mbedtls_ssl_get_output_buflen( &client.ssl ) );
            TEST_ASSERT( client.ssl.in_buf_len ==
                         mbedtls_ssl_get_input_buflen( &client.ssl ) );
            TEST_ASSERT( server.ssl.out_buf_len ==
                         mbedtls_ssl_get_output_buflen( &server.ssl ) );
            TEST_ASSERT( server.ssl.in_buf_len ==
                         mbedtls_ssl_get_input_buflen( &server.ssl ) );
        }
#endif /* MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH */
    }
#endif /* MBEDTLS_SSL_RENEGOTIATION */

exit:
    mbedtls_endpoint_free( &client, options->dtls != 0 ? &client_context : NULL );
    mbedtls_endpoint_free( &server, options->dtls != 0 ? &server_context : NULL );
#if defined (MBEDTLS_DEBUG_C)
    if( options->cli_log_fun || options->srv_log_fun )
    {
        mbedtls_debug_set_threshold( 0 );
    }
#endif
#if defined(MBEDTLS_SSL_CONTEXT_SERIALIZATION)
    if( context_buf != NULL )
        mbedtls_free( context_buf );
#endif
}
#endif /* MBEDTLS_X509_CRT_PARSE_C && MBEDTLS_ENTROPY_C && MBEDTLS_CTR_DRBG_C */

void test_test_callback_buffer_sanity()
{
    enum { MSGLEN = 10 };
    mbedtls_test_buffer buf;
    unsigned char input[MSGLEN];
    unsigned char output[MSGLEN];

    memset( input, 0, sizeof(input) );

    /* Make sure calling put and get on NULL buffer results in error. */
    TEST_ASSERT( mbedtls_test_buffer_put( NULL, input, sizeof( input ) )
                    == -1 );
    TEST_ASSERT( mbedtls_test_buffer_get( NULL, output, sizeof( output ) )
                    == -1 );
    TEST_ASSERT( mbedtls_test_buffer_put( NULL, NULL, sizeof( input ) ) == -1 );

    TEST_ASSERT( mbedtls_test_buffer_put( NULL, NULL, 0 ) == -1 );
    TEST_ASSERT( mbedtls_test_buffer_get( NULL, NULL, 0 ) == -1 );

    /* Make sure calling put and get on a buffer that hasn't been set up results
     * in eror. */
    mbedtls_test_buffer_init( &buf );

    TEST_ASSERT( mbedtls_test_buffer_put( &buf, input, sizeof( input ) ) == -1 );
    TEST_ASSERT( mbedtls_test_buffer_get( &buf, output, sizeof( output ) )
                    == -1 );
    TEST_ASSERT( mbedtls_test_buffer_put( &buf, NULL, sizeof( input ) ) == -1 );

    TEST_ASSERT( mbedtls_test_buffer_put( &buf, NULL, 0 ) == -1 );
    TEST_ASSERT( mbedtls_test_buffer_get( &buf, NULL, 0 ) == -1 );

    /* Make sure calling put and get on NULL input only results in
     * error if the length is not zero, and that a NULL output is valid for data
     * dropping.
     */

    TEST_ASSERT( mbedtls_test_buffer_setup( &buf, sizeof( input ) ) == 0 );

    TEST_ASSERT( mbedtls_test_buffer_put( &buf, NULL, sizeof( input ) ) == -1 );
    TEST_ASSERT( mbedtls_test_buffer_get( &buf, NULL, sizeof( output ) )
                    == 0 );
    TEST_ASSERT( mbedtls_test_buffer_put( &buf, NULL, 0 ) == 0 );
    TEST_ASSERT( mbedtls_test_buffer_get( &buf, NULL, 0 ) == 0 );

    /* Make sure calling put several times in the row is safe */

    TEST_ASSERT( mbedtls_test_buffer_put( &buf, input, sizeof( input ) )
                                          == sizeof( input ) );
    TEST_ASSERT( mbedtls_test_buffer_get( &buf, output, 2 ) == 2 );
    TEST_ASSERT( mbedtls_test_buffer_put( &buf, input, 1 ) == 1 );
    TEST_ASSERT( mbedtls_test_buffer_put( &buf, input, 2 ) == 1 );
    TEST_ASSERT( mbedtls_test_buffer_put( &buf, input, 2 ) == 0 );


exit:

    mbedtls_test_buffer_free( &buf );
}

void test_test_callback_buffer_sanity_wrapper( void ** params )
{
    (void)params;

    test_test_callback_buffer_sanity(  );
}
void test_test_callback_buffer( int size, int put1, int put1_ret,
                           int get1, int get1_ret, int put2, int put2_ret,
                           int get2, int get2_ret )
{
    enum { ROUNDS = 2 };
    size_t put[ROUNDS];
    int put_ret[ROUNDS];
    size_t get[ROUNDS];
    int get_ret[ROUNDS];
    mbedtls_test_buffer buf;
    unsigned char* input = NULL;
    size_t input_len;
    unsigned char* output = NULL;
    size_t output_len;
    size_t i, j, written, read;

    mbedtls_test_buffer_init( &buf );
    TEST_ASSERT( mbedtls_test_buffer_setup( &buf, size ) == 0 );

    /* Check the sanity of input parameters and initialise local variables. That
     * is, ensure that the amount of data is not negative and that we are not
     * expecting more to put or get than we actually asked for. */
    TEST_ASSERT( put1 >= 0 );
    put[0] = put1;
    put_ret[0] = put1_ret;
    TEST_ASSERT( put1_ret <= put1 );
    TEST_ASSERT( put2 >= 0 );
    put[1] = put2;
    put_ret[1] = put2_ret;
    TEST_ASSERT( put2_ret <= put2 );

    TEST_ASSERT( get1 >= 0 );
    get[0] = get1;
    get_ret[0] = get1_ret;
    TEST_ASSERT( get1_ret <= get1 );
    TEST_ASSERT( get2 >= 0 );
    get[1] = get2;
    get_ret[1] = get2_ret;
    TEST_ASSERT( get2_ret <= get2 );

    input_len = 0;
    /* Calculate actual input and output lengths */
    for( j = 0; j < ROUNDS; j++ )
    {
        if( put_ret[j] > 0 )
        {
            input_len += put_ret[j];
        }
    }
    /* In order to always have a valid pointer we always allocate at least 1
     * byte. */
    if( input_len == 0 )
        input_len = 1;
    ASSERT_ALLOC( input, input_len );

    output_len = 0;
    for( j = 0; j < ROUNDS; j++ )
    {
        if( get_ret[j] > 0 )
        {
            output_len += get_ret[j];
        }
    }
    TEST_ASSERT( output_len <= input_len );
    /* In order to always have a valid pointer we always allocate at least 1
     * byte. */
    if( output_len == 0 )
        output_len = 1;
    ASSERT_ALLOC( output, output_len );

    /* Fill up the buffer with structured data so that unwanted changes
     * can be detected */
    for( i = 0; i < input_len; i++ )
    {
        input[i] = i & 0xFF;
    }

    written = read = 0;
    for( j = 0; j < ROUNDS; j++ )
    {
        TEST_ASSERT( put_ret[j] == mbedtls_test_buffer_put( &buf,
                                        input + written, put[j] ) );
        written += put_ret[j];
        TEST_ASSERT( get_ret[j] == mbedtls_test_buffer_get( &buf,
                                        output + read, get[j] ) );
        read += get_ret[j];
        TEST_ASSERT( read <= written );
        if( get_ret[j] > 0 )
        {
            TEST_ASSERT( memcmp( output + read - get_ret[j],
                                 input + read - get_ret[j], get_ret[j] )
                         == 0 );
        }
    }

exit:

    mbedtls_free( input );
    mbedtls_free( output );
    mbedtls_test_buffer_free( &buf );
}

void test_test_callback_buffer_wrapper( void ** params )
{

    test_test_callback_buffer( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ), *( (int *) params[5] ), *( (int *) params[6] ), *( (int *) params[7] ), *( (int *) params[8] ) );
}
void test_ssl_mock_sanity( )
{
    enum { MSGLEN = 105 };
    unsigned char message[MSGLEN];
    unsigned char received[MSGLEN];
    mbedtls_mock_socket socket;

    memset(message, 0, sizeof(message));

    mbedtls_mock_socket_init( &socket );
    TEST_ASSERT( mbedtls_mock_tcp_send_b( &socket, message, MSGLEN ) < 0 );
    mbedtls_mock_socket_close( &socket );
    mbedtls_mock_socket_init( &socket );
    TEST_ASSERT( mbedtls_mock_tcp_recv_b( &socket, received, MSGLEN ) < 0 );
    mbedtls_mock_socket_close( &socket );

    mbedtls_mock_socket_init( &socket );
    TEST_ASSERT( mbedtls_mock_tcp_send_nb( &socket, message, MSGLEN ) < 0 );
    mbedtls_mock_socket_close( &socket );
    mbedtls_mock_socket_init( &socket );
    TEST_ASSERT( mbedtls_mock_tcp_recv_nb( &socket, received, MSGLEN ) < 0 );
    mbedtls_mock_socket_close( &socket );

exit:

    mbedtls_mock_socket_close( &socket );
}

void test_ssl_mock_sanity_wrapper( void ** params )
{
    (void)params;

    test_ssl_mock_sanity(  );
}
void test_ssl_mock_tcp( int blocking )
{
    enum { MSGLEN = 105 };
    enum { BUFLEN = MSGLEN / 5 };
    unsigned char message[MSGLEN];
    unsigned char received[MSGLEN];
    mbedtls_mock_socket client;
    mbedtls_mock_socket server;
    size_t written, read;
    int send_ret, recv_ret;
    mbedtls_ssl_send_t *send;
    mbedtls_ssl_recv_t *recv;
    unsigned i;

    if( blocking == 0 )
    {
        send = mbedtls_mock_tcp_send_nb;
        recv = mbedtls_mock_tcp_recv_nb;
    }
    else
    {
        send = mbedtls_mock_tcp_send_b;
        recv = mbedtls_mock_tcp_recv_b;
    }

    mbedtls_mock_socket_init( &client );
    mbedtls_mock_socket_init( &server );

    /* Fill up the buffer with structured data so that unwanted changes
     * can be detected */
    for( i = 0; i < MSGLEN; i++ )
    {
        message[i] = i & 0xFF;
    }

    /* Make sure that sending a message takes a few  iterations. */
    TEST_ASSERT( 0 == mbedtls_mock_socket_connect( &client, &server, BUFLEN ) );

    /* Send the message to the server */
    send_ret = recv_ret = 1;
    written = read = 0;
    while( send_ret != 0 || recv_ret != 0 )
    {
        send_ret = send( &client, message + written, MSGLEN - written );

        TEST_ASSERT( send_ret >= 0 );
        TEST_ASSERT( send_ret <= BUFLEN );
        written += send_ret;

        /* If the buffer is full we can test blocking and non-blocking send */
        if ( send_ret == BUFLEN )
        {
            int blocking_ret = send( &client, message , 1 );
            if ( blocking )
            {
                TEST_ASSERT( blocking_ret == 0 );
            }
            else
            {
                TEST_ASSERT( blocking_ret == MBEDTLS_ERR_SSL_WANT_WRITE );
            }
        }

        recv_ret = recv( &server, received + read, MSGLEN - read );

        /* The result depends on whether any data was sent */
        if ( send_ret > 0 )
        {
            TEST_ASSERT( recv_ret > 0 );
            TEST_ASSERT( recv_ret <= BUFLEN );
            read += recv_ret;
        }
        else if( blocking )
        {
            TEST_ASSERT( recv_ret == 0 );
        }
        else
        {
            TEST_ASSERT( recv_ret == MBEDTLS_ERR_SSL_WANT_READ );
            recv_ret = 0;
        }

        /* If the buffer is empty we can test blocking and non-blocking read */
        if ( recv_ret == BUFLEN )
        {
            int blocking_ret = recv( &server, received, 1 );
            if ( blocking )
            {
                TEST_ASSERT( blocking_ret == 0 );
            }
            else
            {
                TEST_ASSERT( blocking_ret == MBEDTLS_ERR_SSL_WANT_READ );
            }
        }
    }
    TEST_ASSERT( memcmp( message, received, MSGLEN ) == 0 );

exit:

    mbedtls_mock_socket_close( &client );
    mbedtls_mock_socket_close( &server );
}

void test_ssl_mock_tcp_wrapper( void ** params )
{

    test_ssl_mock_tcp( *( (int *) params[0] ) );
}
void test_ssl_mock_tcp_interleaving( int blocking )
{
    enum { ROUNDS = 2 };
    enum { MSGLEN = 105 };
    enum { BUFLEN = MSGLEN / 5 };
    unsigned char message[ROUNDS][MSGLEN];
    unsigned char received[ROUNDS][MSGLEN];
    mbedtls_mock_socket client;
    mbedtls_mock_socket server;
    size_t written[ROUNDS];
    size_t read[ROUNDS];
    int send_ret[ROUNDS];
    int recv_ret[ROUNDS];
    unsigned i, j, progress;
    mbedtls_ssl_send_t *send;
    mbedtls_ssl_recv_t *recv;

    if( blocking == 0 )
    {
        send = mbedtls_mock_tcp_send_nb;
        recv = mbedtls_mock_tcp_recv_nb;
    }
    else
    {
        send = mbedtls_mock_tcp_send_b;
        recv = mbedtls_mock_tcp_recv_b;
    }

    mbedtls_mock_socket_init( &client );
    mbedtls_mock_socket_init( &server );

    /* Fill up the buffers with structured data so that unwanted changes
     * can be detected */
    for( i = 0; i < ROUNDS; i++ )
    {
        for( j = 0; j < MSGLEN; j++ )
        {
            message[i][j] = ( i * MSGLEN + j ) & 0xFF;
        }
    }

    /* Make sure that sending a message takes a few  iterations. */
    TEST_ASSERT( 0 == mbedtls_mock_socket_connect( &client, &server, BUFLEN ) );

    /* Send the message from both sides, interleaving. */
    progress = 1;
    for( i = 0; i < ROUNDS; i++ )
    {
        written[i] = 0;
        read[i] = 0;
    }
    /* This loop does not stop as long as there was a successful write or read
     * of at least one byte on either side. */
    while( progress != 0 )
    {
        mbedtls_mock_socket *socket;

        for( i = 0; i < ROUNDS; i++ )
        {
            /* First sending is from the client */
            socket = ( i % 2 == 0 ) ? ( &client ) : ( &server );

            send_ret[i] = send( socket, message[i] + written[i],
                                               MSGLEN - written[i] );
            TEST_ASSERT( send_ret[i] >= 0 );
            TEST_ASSERT( send_ret[i] <= BUFLEN );
            written[i] += send_ret[i];

            /* If the buffer is full we can test blocking and non-blocking
             * send */
            if ( send_ret[i] == BUFLEN )
            {
                int blocking_ret = send( socket, message[i] , 1 );
                if ( blocking )
                {
                    TEST_ASSERT( blocking_ret == 0 );
                }
                else
                {
                    TEST_ASSERT( blocking_ret == MBEDTLS_ERR_SSL_WANT_WRITE );
                }
            }
        }

        for( i = 0; i < ROUNDS; i++ )
        {
            /* First receiving is from the server */
            socket = ( i % 2 == 0 ) ? ( &server ) : ( &client );

            recv_ret[i] = recv( socket, received[i] + read[i],
                                               MSGLEN - read[i] );

            /* The result depends on whether any data was sent */
            if ( send_ret[i] > 0 )
            {
                TEST_ASSERT( recv_ret[i] > 0 );
                TEST_ASSERT( recv_ret[i] <= BUFLEN );
                read[i] += recv_ret[i];
            }
            else if( blocking )
            {
                TEST_ASSERT( recv_ret[i] == 0 );
            }
            else
            {
                TEST_ASSERT( recv_ret[i] == MBEDTLS_ERR_SSL_WANT_READ );
                recv_ret[i] = 0;
            }

            /* If the buffer is empty we can test blocking and non-blocking
             * read */
            if ( recv_ret[i] == BUFLEN )
            {
                int blocking_ret = recv( socket, received[i], 1 );
                if ( blocking )
                {
                    TEST_ASSERT( blocking_ret == 0 );
                }
                else
                {
                    TEST_ASSERT( blocking_ret == MBEDTLS_ERR_SSL_WANT_READ );
                }
            }
        }

        progress = 0;
        for( i = 0; i < ROUNDS; i++ )
        {
            progress += send_ret[i] + recv_ret[i];
        }
    }

    for( i = 0; i < ROUNDS; i++ )
        TEST_ASSERT( memcmp( message[i], received[i], MSGLEN ) == 0 );

exit:

    mbedtls_mock_socket_close( &client );
    mbedtls_mock_socket_close( &server );
}

void test_ssl_mock_tcp_interleaving_wrapper( void ** params )
{

    test_ssl_mock_tcp_interleaving( *( (int *) params[0] ) );
}
void test_ssl_message_queue_sanity( )
{
    mbedtls_test_message_queue queue;

    /* Trying to push/pull to an empty queue */
    TEST_ASSERT( mbedtls_test_message_queue_push_info( NULL, 1 )
                 == MBEDTLS_TEST_ERROR_ARG_NULL );
    TEST_ASSERT( mbedtls_test_message_queue_pop_info( NULL, 1 )
                 == MBEDTLS_TEST_ERROR_ARG_NULL );

    TEST_ASSERT( mbedtls_test_message_queue_setup( &queue, 3 ) == 0 );
    TEST_ASSERT( queue.capacity == 3 );
    TEST_ASSERT( queue.num == 0 );

exit:
    mbedtls_test_message_queue_free( &queue );
}

void test_ssl_message_queue_sanity_wrapper( void ** params )
{
    (void)params;

    test_ssl_message_queue_sanity(  );
}
void test_ssl_message_queue_basic( )
{
    mbedtls_test_message_queue queue;

    TEST_ASSERT( mbedtls_test_message_queue_setup( &queue, 3 ) == 0 );

    /* Sanity test - 3 pushes and 3 pops with sufficient space */
    TEST_ASSERT( mbedtls_test_message_queue_push_info( &queue, 1 ) == 1 );
    TEST_ASSERT( queue.capacity == 3 );
    TEST_ASSERT( queue.num == 1 );
    TEST_ASSERT( mbedtls_test_message_queue_push_info( &queue, 1 ) == 1 );
    TEST_ASSERT( queue.capacity == 3 );
    TEST_ASSERT( queue.num == 2 );
    TEST_ASSERT( mbedtls_test_message_queue_push_info( &queue, 2 ) == 2 );
    TEST_ASSERT( queue.capacity == 3 );
    TEST_ASSERT( queue.num == 3 );

    TEST_ASSERT( mbedtls_test_message_queue_pop_info( &queue, 1 ) == 1 );
    TEST_ASSERT( mbedtls_test_message_queue_pop_info( &queue, 1 ) == 1 );
    TEST_ASSERT( mbedtls_test_message_queue_pop_info( &queue, 2 ) == 2 );

exit:
    mbedtls_test_message_queue_free( &queue );
}

void test_ssl_message_queue_basic_wrapper( void ** params )
{
    (void)params;

    test_ssl_message_queue_basic(  );
}
void test_ssl_message_queue_overflow_underflow( )
{
    mbedtls_test_message_queue queue;

    TEST_ASSERT( mbedtls_test_message_queue_setup( &queue, 3 ) == 0 );

    /* 4 pushes (last one with an error), 4 pops (last one with an error) */
    TEST_ASSERT( mbedtls_test_message_queue_push_info( &queue, 1 ) == 1 );
    TEST_ASSERT( mbedtls_test_message_queue_push_info( &queue, 1 ) == 1 );
    TEST_ASSERT( mbedtls_test_message_queue_push_info( &queue, 2 ) == 2 );
    TEST_ASSERT( mbedtls_test_message_queue_push_info( &queue, 3 )
                 == MBEDTLS_ERR_SSL_WANT_WRITE );

    TEST_ASSERT( mbedtls_test_message_queue_pop_info( &queue, 1 ) == 1 );
    TEST_ASSERT( mbedtls_test_message_queue_pop_info( &queue, 1 ) == 1 );
    TEST_ASSERT( mbedtls_test_message_queue_pop_info( &queue, 2 ) == 2 );

    TEST_ASSERT( mbedtls_test_message_queue_pop_info( &queue, 1 )
                 == MBEDTLS_ERR_SSL_WANT_READ );

exit:
    mbedtls_test_message_queue_free( &queue );
}

void test_ssl_message_queue_overflow_underflow_wrapper( void ** params )
{
    (void)params;

    test_ssl_message_queue_overflow_underflow(  );
}
void test_ssl_message_queue_interleaved( )
{
    mbedtls_test_message_queue queue;

    TEST_ASSERT( mbedtls_test_message_queue_setup( &queue, 3 ) == 0 );

    /* Interleaved test - [2 pushes, 1 pop] twice, and then two pops
     * (to wrap around the buffer) */
    TEST_ASSERT( mbedtls_test_message_queue_push_info( &queue, 1 ) == 1 );
    TEST_ASSERT( mbedtls_test_message_queue_push_info( &queue, 1 ) == 1 );

    TEST_ASSERT( mbedtls_test_message_queue_pop_info( &queue, 1 ) == 1 );

    TEST_ASSERT( mbedtls_test_message_queue_push_info( &queue, 2 ) == 2 );
    TEST_ASSERT( mbedtls_test_message_queue_push_info( &queue, 3 ) == 3 );

    TEST_ASSERT( mbedtls_test_message_queue_pop_info( &queue, 1 ) == 1 );
    TEST_ASSERT( mbedtls_test_message_queue_pop_info( &queue, 2 ) == 2 );

    TEST_ASSERT( mbedtls_test_message_queue_push_info( &queue, 5 ) == 5 );
    TEST_ASSERT( mbedtls_test_message_queue_push_info( &queue, 8 ) == 8 );

    TEST_ASSERT( mbedtls_test_message_queue_pop_info( &queue, 3 ) == 3 );

    TEST_ASSERT( mbedtls_test_message_queue_pop_info( &queue, 5 ) == 5 );

    TEST_ASSERT( mbedtls_test_message_queue_pop_info( &queue, 8 ) == 8 );

exit:
    mbedtls_test_message_queue_free( &queue );
}

void test_ssl_message_queue_interleaved_wrapper( void ** params )
{
    (void)params;

    test_ssl_message_queue_interleaved(  );
}
void test_ssl_message_queue_insufficient_buffer( )
{
    mbedtls_test_message_queue queue;
    size_t message_len = 10;
    size_t buffer_len = 5;

    TEST_ASSERT( mbedtls_test_message_queue_setup( &queue, 1 ) == 0 );

    /* Popping without a sufficient buffer */
    TEST_ASSERT( mbedtls_test_message_queue_push_info( &queue, message_len )
                 == (int) message_len );
    TEST_ASSERT( mbedtls_test_message_queue_pop_info( &queue, buffer_len )
                 == (int) buffer_len );
exit:
    mbedtls_test_message_queue_free( &queue );
}

void test_ssl_message_queue_insufficient_buffer_wrapper( void ** params )
{
    (void)params;

    test_ssl_message_queue_insufficient_buffer(  );
}
void test_ssl_message_mock_uninitialized( )
{
    enum { MSGLEN = 10 };
    unsigned char message[MSGLEN] = {0}, received[MSGLEN];
    mbedtls_mock_socket client, server;
    mbedtls_test_message_queue server_queue, client_queue;
    mbedtls_test_message_socket_context server_context, client_context;
    mbedtls_message_socket_init( &server_context );
    mbedtls_message_socket_init( &client_context );

    /* Send with a NULL context */
    TEST_ASSERT( mbedtls_mock_tcp_send_msg( NULL, message, MSGLEN )
                 == MBEDTLS_TEST_ERROR_CONTEXT_ERROR );

    TEST_ASSERT( mbedtls_mock_tcp_recv_msg( NULL, message, MSGLEN )
                 == MBEDTLS_TEST_ERROR_CONTEXT_ERROR );

    TEST_ASSERT( mbedtls_message_socket_setup( &server_queue, &client_queue, 1,
                                               &server,
                                               &server_context ) == 0 );

    TEST_ASSERT( mbedtls_message_socket_setup( &client_queue, &server_queue, 1,
                                               &client,
                                               &client_context ) == 0 );

    TEST_ASSERT( mbedtls_mock_tcp_send_msg( &client_context, message, MSGLEN )
                 == MBEDTLS_TEST_ERROR_SEND_FAILED );

    TEST_ASSERT( mbedtls_mock_tcp_recv_msg( &server_context, received, MSGLEN )
                 == MBEDTLS_ERR_SSL_WANT_READ );

    /* Push directly to a queue to later simulate a disconnected behavior */
    TEST_ASSERT( mbedtls_test_message_queue_push_info( &server_queue, MSGLEN )
                 == MSGLEN );

    /* Test if there's an error when trying to read from a disconnected
     * socket */
    TEST_ASSERT( mbedtls_mock_tcp_recv_msg( &server_context, received, MSGLEN )
                 == MBEDTLS_TEST_ERROR_RECV_FAILED );
    exit:
    mbedtls_message_socket_close( &server_context );
    mbedtls_message_socket_close( &client_context );
}

void test_ssl_message_mock_uninitialized_wrapper( void ** params )
{
    (void)params;

    test_ssl_message_mock_uninitialized(  );
}
void test_ssl_message_mock_basic( )
{
    enum { MSGLEN = 10 };
    unsigned char message[MSGLEN], received[MSGLEN];
    mbedtls_mock_socket client, server;
    unsigned i;
    mbedtls_test_message_queue server_queue, client_queue;
    mbedtls_test_message_socket_context server_context, client_context;
    mbedtls_message_socket_init( &server_context );
    mbedtls_message_socket_init( &client_context );

    TEST_ASSERT( mbedtls_message_socket_setup( &server_queue, &client_queue, 1,
                                               &server,
                                               &server_context ) == 0 );

    TEST_ASSERT( mbedtls_message_socket_setup( &client_queue, &server_queue, 1,
                                               &client,
                                               &client_context ) == 0 );

    /* Fill up the buffer with structured data so that unwanted changes
     * can be detected */
    for( i = 0; i < MSGLEN; i++ )
    {
        message[i] = i & 0xFF;
    }
    TEST_ASSERT( 0 == mbedtls_mock_socket_connect( &client, &server,
                                                   MSGLEN ) );

    /* Send the message to the server */
    TEST_ASSERT( mbedtls_mock_tcp_send_msg( &client_context, message,
                                            MSGLEN ) == MSGLEN );

    /* Read from the server */
    TEST_ASSERT( mbedtls_mock_tcp_recv_msg( &server_context, received, MSGLEN )
                 == MSGLEN );

    TEST_ASSERT( memcmp( message, received, MSGLEN ) == 0 );
    memset( received, 0, MSGLEN );

    /* Send the message to the client */
    TEST_ASSERT( mbedtls_mock_tcp_send_msg( &server_context, message,
                                            MSGLEN ) == MSGLEN );

    /* Read from the client */
    TEST_ASSERT( mbedtls_mock_tcp_recv_msg( &client_context, received, MSGLEN )
                 == MSGLEN );
    TEST_ASSERT( memcmp( message, received, MSGLEN ) == 0 );

    exit:
    mbedtls_message_socket_close( &server_context );
    mbedtls_message_socket_close( &client_context );
}

void test_ssl_message_mock_basic_wrapper( void ** params )
{
    (void)params;

    test_ssl_message_mock_basic(  );
}
void test_ssl_message_mock_queue_overflow_underflow( )
{
    enum { MSGLEN = 10 };
    unsigned char message[MSGLEN], received[MSGLEN];
    mbedtls_mock_socket client, server;
    unsigned i;
    mbedtls_test_message_queue server_queue, client_queue;
    mbedtls_test_message_socket_context server_context, client_context;
    mbedtls_message_socket_init( &server_context );
    mbedtls_message_socket_init( &client_context );

    TEST_ASSERT( mbedtls_message_socket_setup( &server_queue, &client_queue, 2,
                                               &server,
                                               &server_context ) == 0 );

    TEST_ASSERT( mbedtls_message_socket_setup( &client_queue, &server_queue, 2,
                                               &client,
                                               &client_context ) == 0 );

    /* Fill up the buffer with structured data so that unwanted changes
     * can be detected */
    for( i = 0; i < MSGLEN; i++ )
    {
        message[i] = i & 0xFF;
    }
    TEST_ASSERT( 0 == mbedtls_mock_socket_connect( &client, &server,
                                                   MSGLEN*2 ) );

    /* Send three message to the server, last one with an error */
    TEST_ASSERT( mbedtls_mock_tcp_send_msg( &client_context, message,
                                            MSGLEN - 1 ) == MSGLEN - 1 );

    TEST_ASSERT( mbedtls_mock_tcp_send_msg( &client_context, message,
                                            MSGLEN ) == MSGLEN );

    TEST_ASSERT( mbedtls_mock_tcp_send_msg( &client_context, message,
                                            MSGLEN )
                 == MBEDTLS_ERR_SSL_WANT_WRITE );

    /* Read three messages from the server, last one with an error */
    TEST_ASSERT( mbedtls_mock_tcp_recv_msg( &server_context, received,
                                            MSGLEN - 1 ) == MSGLEN - 1 );

    TEST_ASSERT( mbedtls_mock_tcp_recv_msg( &server_context, received, MSGLEN )
                 == MSGLEN );

    TEST_ASSERT( memcmp( message, received, MSGLEN ) == 0 );

    TEST_ASSERT( mbedtls_mock_tcp_recv_msg( &server_context, received, MSGLEN )
                 == MBEDTLS_ERR_SSL_WANT_READ );

    exit:
    mbedtls_message_socket_close( &server_context );
    mbedtls_message_socket_close( &client_context );
}

void test_ssl_message_mock_queue_overflow_underflow_wrapper( void ** params )
{
    (void)params;

    test_ssl_message_mock_queue_overflow_underflow(  );
}
void test_ssl_message_mock_socket_overflow( )
{
    enum { MSGLEN = 10 };
    unsigned char message[MSGLEN], received[MSGLEN];
    mbedtls_mock_socket client, server;
    unsigned i;
    mbedtls_test_message_queue server_queue, client_queue;
    mbedtls_test_message_socket_context server_context, client_context;
    mbedtls_message_socket_init( &server_context );
    mbedtls_message_socket_init( &client_context );

    TEST_ASSERT( mbedtls_message_socket_setup( &server_queue, &client_queue, 2,
                                               &server,
                                               &server_context ) == 0 );

    TEST_ASSERT( mbedtls_message_socket_setup( &client_queue, &server_queue, 2,
                                               &client,
                                               &client_context ) == 0 );

    /* Fill up the buffer with structured data so that unwanted changes
     * can be detected */
    for( i = 0; i < MSGLEN; i++ )
    {
        message[i] = i & 0xFF;
    }
    TEST_ASSERT( 0 == mbedtls_mock_socket_connect( &client, &server,
                                                   MSGLEN ) );

    /* Send two message to the server, second one with an error */
    TEST_ASSERT( mbedtls_mock_tcp_send_msg( &client_context, message,
                                            MSGLEN ) == MSGLEN );

    TEST_ASSERT( mbedtls_mock_tcp_send_msg( &client_context, message,
                                            MSGLEN )
                 == MBEDTLS_TEST_ERROR_SEND_FAILED );

    /* Read the only message from the server */
    TEST_ASSERT( mbedtls_mock_tcp_recv_msg( &server_context, received, MSGLEN )
                 == MSGLEN );

    TEST_ASSERT( memcmp( message, received, MSGLEN ) == 0 );

    exit:
    mbedtls_message_socket_close( &server_context );
    mbedtls_message_socket_close( &client_context );
}

void test_ssl_message_mock_socket_overflow_wrapper( void ** params )
{
    (void)params;

    test_ssl_message_mock_socket_overflow(  );
}
void test_ssl_message_mock_truncated( )
{
    enum { MSGLEN = 10 };
    unsigned char message[MSGLEN], received[MSGLEN];
    mbedtls_mock_socket client, server;
    unsigned i;
    mbedtls_test_message_queue server_queue, client_queue;
    mbedtls_test_message_socket_context server_context, client_context;
    mbedtls_message_socket_init( &server_context );
    mbedtls_message_socket_init( &client_context );

    TEST_ASSERT( mbedtls_message_socket_setup( &server_queue, &client_queue, 2,
                                               &server,
                                               &server_context ) == 0 );

    TEST_ASSERT( mbedtls_message_socket_setup( &client_queue, &server_queue, 2,
                                               &client,
                                               &client_context ) == 0 );

    memset( received, 0, MSGLEN );
    /* Fill up the buffer with structured data so that unwanted changes
     * can be detected */
    for( i = 0; i < MSGLEN; i++ )
    {
        message[i] = i & 0xFF;
    }
    TEST_ASSERT( 0 == mbedtls_mock_socket_connect( &client, &server,
                                                   2 * MSGLEN ) );

    /* Send two messages to the server, the second one small enough to fit in the
     * receiver's buffer. */
    TEST_ASSERT( mbedtls_mock_tcp_send_msg( &client_context, message,
                                            MSGLEN ) == MSGLEN );
    TEST_ASSERT( mbedtls_mock_tcp_send_msg( &client_context, message,
                                            MSGLEN / 2 ) == MSGLEN / 2 );
    /* Read a truncated message from the server */
    TEST_ASSERT( mbedtls_mock_tcp_recv_msg( &server_context, received, MSGLEN/2 )
                 == MSGLEN/2 );

    /* Test that the first half of the message is valid, and second one isn't */
    TEST_ASSERT( memcmp( message, received, MSGLEN/2 ) == 0 );
    TEST_ASSERT( memcmp( message + MSGLEN/2, received + MSGLEN/2, MSGLEN/2 )
                 != 0 );
    memset( received, 0, MSGLEN );

    /* Read a full message from the server */
    TEST_ASSERT( mbedtls_mock_tcp_recv_msg( &server_context, received, MSGLEN/2 )
                 == MSGLEN / 2 );

    /* Test that the first half of the message is valid */
    TEST_ASSERT( memcmp( message, received, MSGLEN/2 ) == 0 );

    exit:
    mbedtls_message_socket_close( &server_context );
    mbedtls_message_socket_close( &client_context );
}

void test_ssl_message_mock_truncated_wrapper( void ** params )
{
    (void)params;

    test_ssl_message_mock_truncated(  );
}
void test_ssl_message_mock_socket_read_error( )
{
    enum { MSGLEN = 10 };
    unsigned char message[MSGLEN], received[MSGLEN];
    mbedtls_mock_socket client, server;
    unsigned i;
    mbedtls_test_message_queue server_queue, client_queue;
    mbedtls_test_message_socket_context server_context, client_context;
    mbedtls_message_socket_init( &server_context );
    mbedtls_message_socket_init( &client_context );

    TEST_ASSERT( mbedtls_message_socket_setup( &server_queue, &client_queue, 1,
                                               &server,
                                               &server_context ) == 0 );

    TEST_ASSERT( mbedtls_message_socket_setup( &client_queue, &server_queue, 1,
                                               &client,
                                               &client_context ) == 0 );

    /* Fill up the buffer with structured data so that unwanted changes
     * can be detected */
    for( i = 0; i < MSGLEN; i++ )
    {
        message[i] = i & 0xFF;
    }
    TEST_ASSERT( 0 == mbedtls_mock_socket_connect( &client, &server,
                                                   MSGLEN ) );

    TEST_ASSERT( mbedtls_mock_tcp_send_msg( &client_context, message,
                                            MSGLEN ) == MSGLEN );

    /* Force a read error by disconnecting the socket by hand */
    server.status = 0;
    TEST_ASSERT( mbedtls_mock_tcp_recv_msg( &server_context, received, MSGLEN )
                 == MBEDTLS_TEST_ERROR_RECV_FAILED );
    /* Return to a valid state */
    server.status = MBEDTLS_MOCK_SOCKET_CONNECTED;

    memset( received, 0, sizeof( received ) );

    /* Test that even though the server tried to read once disconnected, the
     * continuity is preserved */
    TEST_ASSERT( mbedtls_mock_tcp_recv_msg( &server_context, received, MSGLEN )
                 == MSGLEN );

    TEST_ASSERT( memcmp( message, received, MSGLEN ) == 0 );

    exit:
    mbedtls_message_socket_close( &server_context );
    mbedtls_message_socket_close( &client_context );
}

void test_ssl_message_mock_socket_read_error_wrapper( void ** params )
{
    (void)params;

    test_ssl_message_mock_socket_read_error(  );
}
void test_ssl_message_mock_interleaved_one_way( )
{
    enum { MSGLEN = 10 };
    unsigned char message[MSGLEN], received[MSGLEN];
    mbedtls_mock_socket client, server;
    unsigned i;
    mbedtls_test_message_queue server_queue, client_queue;
    mbedtls_test_message_socket_context server_context, client_context;
    mbedtls_message_socket_init( &server_context );
    mbedtls_message_socket_init( &client_context );

    TEST_ASSERT( mbedtls_message_socket_setup( &server_queue, &client_queue, 3,
                                               &server,
                                               &server_context ) == 0 );

    TEST_ASSERT( mbedtls_message_socket_setup( &client_queue, &server_queue, 3,
                                               &client,
                                               &client_context ) == 0 );

    /* Fill up the buffer with structured data so that unwanted changes
     * can be detected */
    for( i = 0; i < MSGLEN; i++ )
    {
        message[i] = i & 0xFF;
    }
    TEST_ASSERT( 0 == mbedtls_mock_socket_connect( &client, &server,
                                                   MSGLEN*3 ) );

    /* Interleaved test - [2 sends, 1 read] twice, and then two reads
     * (to wrap around the buffer) */
    for( i = 0; i < 2; i++ )
    {
        TEST_ASSERT( mbedtls_mock_tcp_send_msg( &client_context, message,
                                                MSGLEN ) == MSGLEN );

        TEST_ASSERT( mbedtls_mock_tcp_send_msg( &client_context, message,
                                                MSGLEN ) == MSGLEN );

        TEST_ASSERT( mbedtls_mock_tcp_recv_msg( &server_context, received,
                                                MSGLEN ) == MSGLEN );
        TEST_ASSERT( memcmp( message, received, MSGLEN ) == 0 );
        memset( received, 0, sizeof( received ) );
    }

    for( i = 0; i < 2; i++ )
    {
        TEST_ASSERT( mbedtls_mock_tcp_recv_msg( &server_context, received,
                                                MSGLEN ) == MSGLEN );

        TEST_ASSERT( memcmp( message, received, MSGLEN ) == 0 );
    }
    TEST_ASSERT( mbedtls_mock_tcp_recv_msg( &server_context, received, MSGLEN )
                 == MBEDTLS_ERR_SSL_WANT_READ );
    exit:
    mbedtls_message_socket_close( &server_context );
    mbedtls_message_socket_close( &client_context );
}

void test_ssl_message_mock_interleaved_one_way_wrapper( void ** params )
{
    (void)params;

    test_ssl_message_mock_interleaved_one_way(  );
}
void test_ssl_message_mock_interleaved_two_ways( )
{
    enum { MSGLEN = 10 };
    unsigned char message[MSGLEN], received[MSGLEN];
    mbedtls_mock_socket client, server;
    unsigned i;
    mbedtls_test_message_queue server_queue, client_queue;
    mbedtls_test_message_socket_context server_context, client_context;
    mbedtls_message_socket_init( &server_context );
    mbedtls_message_socket_init( &client_context );

    TEST_ASSERT( mbedtls_message_socket_setup( &server_queue, &client_queue, 3,
                                               &server,
                                               &server_context ) == 0 );

    TEST_ASSERT( mbedtls_message_socket_setup( &client_queue, &server_queue, 3,
                                               &client,
                                               &client_context ) == 0 );

    /* Fill up the buffer with structured data so that unwanted changes
     * can be detected */
    for( i = 0; i < MSGLEN; i++ )
    {
        message[i] = i & 0xFF;
    }
    TEST_ASSERT( 0 == mbedtls_mock_socket_connect( &client, &server,
                                                   MSGLEN*3 ) );

    /* Interleaved test - [2 sends, 1 read] twice, both ways, and then two reads
     * (to wrap around the buffer) both ways. */
    for( i = 0; i < 2; i++ )
    {
        TEST_ASSERT( mbedtls_mock_tcp_send_msg( &client_context, message,
                                                MSGLEN ) == MSGLEN );

        TEST_ASSERT( mbedtls_mock_tcp_send_msg( &client_context, message,
                                                MSGLEN ) == MSGLEN );

        TEST_ASSERT( mbedtls_mock_tcp_send_msg( &server_context, message,
                                                MSGLEN ) == MSGLEN );

        TEST_ASSERT( mbedtls_mock_tcp_send_msg( &server_context, message,
                                                MSGLEN ) == MSGLEN );

        TEST_ASSERT( mbedtls_mock_tcp_recv_msg( &server_context, received,
                     MSGLEN ) == MSGLEN );

        TEST_ASSERT( memcmp( message, received, MSGLEN ) == 0 );

        memset( received, 0, sizeof( received ) );

        TEST_ASSERT( mbedtls_mock_tcp_recv_msg( &client_context, received,
                     MSGLEN ) == MSGLEN );

        TEST_ASSERT( memcmp( message, received, MSGLEN ) == 0 );

        memset( received, 0, sizeof( received ) );
    }

    for( i = 0; i < 2; i++ )
    {
        TEST_ASSERT( mbedtls_mock_tcp_recv_msg( &server_context, received,
                    MSGLEN ) == MSGLEN );

        TEST_ASSERT( memcmp( message, received, MSGLEN ) == 0 );
        memset( received, 0, sizeof( received ) );

        TEST_ASSERT( mbedtls_mock_tcp_recv_msg( &client_context, received,
                     MSGLEN ) == MSGLEN );

        TEST_ASSERT( memcmp( message, received, MSGLEN ) == 0 );
        memset( received, 0, sizeof( received ) );
    }

    TEST_ASSERT( mbedtls_mock_tcp_recv_msg( &server_context, received, MSGLEN )
                 == MBEDTLS_ERR_SSL_WANT_READ );

    TEST_ASSERT( mbedtls_mock_tcp_recv_msg( &client_context, received, MSGLEN )
                 == MBEDTLS_ERR_SSL_WANT_READ );
    exit:
    mbedtls_message_socket_close( &server_context );
    mbedtls_message_socket_close( &client_context );
}

void test_ssl_message_mock_interleaved_two_ways_wrapper( void ** params )
{
    (void)params;

    test_ssl_message_mock_interleaved_two_ways(  );
}
#if defined(MBEDTLS_SSL_DTLS_ANTI_REPLAY)
void test_ssl_dtls_replay( data_t * prevs, data_t * new, int ret )
{
    uint32_t len = 0;
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;

    mbedtls_ssl_init( &ssl );
    mbedtls_ssl_config_init( &conf );

    TEST_ASSERT( mbedtls_ssl_config_defaults( &conf,
                 MBEDTLS_SSL_IS_CLIENT,
                 MBEDTLS_SSL_TRANSPORT_DATAGRAM,
                 MBEDTLS_SSL_PRESET_DEFAULT ) == 0 );
    TEST_ASSERT( mbedtls_ssl_setup( &ssl, &conf ) == 0 );

    /* Read previous record numbers */
    for( len = 0; len < prevs->len; len += 6 )
    {
        memcpy( ssl.in_ctr + 2, prevs->x + len, 6 );
        mbedtls_ssl_dtls_replay_update( &ssl );
    }

    /* Check new number */
    memcpy( ssl.in_ctr + 2, new->x, 6 );
    TEST_ASSERT( mbedtls_ssl_dtls_replay_check( &ssl ) == ret );

    mbedtls_ssl_free( &ssl );
    mbedtls_ssl_config_free( &conf );
exit:
    ;
}

void test_ssl_dtls_replay_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};

    test_ssl_dtls_replay( &data0, &data2, *( (int *) params[4] ) );
}
#endif /* MBEDTLS_SSL_DTLS_ANTI_REPLAY */
#if defined(MBEDTLS_X509_CRT_PARSE_C)
void test_ssl_set_hostname_twice( char *hostname0, char *hostname1 )
{
    mbedtls_ssl_context ssl;
    mbedtls_ssl_init( &ssl );

    TEST_ASSERT( mbedtls_ssl_set_hostname( &ssl, hostname0 ) == 0 );
    TEST_ASSERT( mbedtls_ssl_set_hostname( &ssl, hostname1 ) == 0 );

    mbedtls_ssl_free( &ssl );
exit:
    ;
}

void test_ssl_set_hostname_twice_wrapper( void ** params )
{

    test_ssl_set_hostname_twice( (char *) params[0], (char *) params[1] );
}
#endif /* MBEDTLS_X509_CRT_PARSE_C */
void test_ssl_crypt_record( int cipher_type, int hash_id,
                       int etm, int tag_mode, int ver,
                       int cid0_len, int cid1_len )
{
    /*
     * Test several record encryptions and decryptions
     * with plenty of space before and after the data
     * within the record buffer.
     */

    int ret;
    int num_records = 16;
    mbedtls_ssl_context ssl; /* ONLY for debugging */

    mbedtls_ssl_transform t0, t1;
    unsigned char *buf = NULL;
    size_t const buflen = 512;
    mbedtls_record rec, rec_backup;

    mbedtls_ssl_init( &ssl );
    mbedtls_ssl_transform_init( &t0 );
    mbedtls_ssl_transform_init( &t1 );
    TEST_ASSERT( build_transforms( &t0, &t1, cipher_type, hash_id,
                                   etm, tag_mode, ver,
                                   (size_t) cid0_len,
                                   (size_t) cid1_len ) == 0 );

    TEST_ASSERT( ( buf = mbedtls_calloc( 1, buflen ) ) != NULL );

    while( num_records-- > 0 )
    {
        mbedtls_ssl_transform *t_dec, *t_enc;
        /* Take turns in who's sending and who's receiving. */
        if( num_records % 3 == 0 )
        {
            t_dec = &t0;
            t_enc = &t1;
        }
        else
        {
            t_dec = &t1;
            t_enc = &t0;
        }

        /*
         * The record header affects the transformation in two ways:
         * 1) It determines the AEAD additional data
         * 2) The record counter sometimes determines the IV.
         *
         * Apart from that, the fields don't have influence.
         * In particular, it is currently not the responsibility
         * of ssl_encrypt/decrypt_buf to check if the transform
         * version matches the record version, or that the
         * type is sensible.
         */

        memset( rec.ctr, num_records, sizeof( rec.ctr ) );
        rec.type    = 42;
        rec.ver[0]  = num_records;
        rec.ver[1]  = num_records;
#if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
        rec.cid_len = 0;
#endif /* MBEDTLS_SSL_DTLS_CONNECTION_ID */

        rec.buf     = buf;
        rec.buf_len = buflen;
        rec.data_offset = 16;
        /* Make sure to vary the length to exercise different
         * paddings. */
        rec.data_len = 1 + num_records;

        memset( rec.buf + rec.data_offset, 42, rec.data_len );

        /* Make a copy for later comparison */
        rec_backup = rec;

        /* Encrypt record */
        ret = mbedtls_ssl_encrypt_buf( &ssl, t_enc, &rec,
                                       mbedtls_test_rnd_std_rand, NULL );
        TEST_ASSERT( ret == 0 || ret == MBEDTLS_ERR_SSL_BUFFER_TOO_SMALL );
        if( ret != 0 )
        {
            continue;
        }

#if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
        if( rec.cid_len != 0 )
        {
            /* DTLS 1.2 + CID hides the real content type and
             * uses a special CID content type in the protected
             * record. Double-check this. */
            TEST_ASSERT( rec.type == MBEDTLS_SSL_MSG_CID );
        }
#endif /* MBEDTLS_SSL_DTLS_CONNECTION_ID */

#if defined(MBEDTLS_SSL_PROTO_TLS1_3_EXPERIMENTAL)
        if( t_enc->minor_ver == MBEDTLS_SSL_MINOR_VERSION_4 )
        {
            /* TLS 1.3 hides the real content type and
             * always uses Application Data as the content type
             * for protected records. Double-check this. */
            TEST_ASSERT( rec.type == MBEDTLS_SSL_MSG_APPLICATION_DATA );
        }
#endif /* MBEDTLS_SSL_PROTO_TLS1_3_EXPERIMENTAL */

        /* Decrypt record with t_dec */
        TEST_EQUAL( 0, mbedtls_ssl_decrypt_buf( &ssl, t_dec, &rec ) );

        /* Compare results */
        TEST_ASSERT( rec.type == rec_backup.type );
        TEST_ASSERT( memcmp( rec.ctr, rec_backup.ctr, 8 ) == 0 );
        TEST_ASSERT( rec.ver[0] == rec_backup.ver[0] );
        TEST_ASSERT( rec.ver[1] == rec_backup.ver[1] );
        TEST_ASSERT( rec.data_len == rec_backup.data_len );
        TEST_ASSERT( rec.data_offset == rec_backup.data_offset );
        TEST_ASSERT( memcmp( rec.buf + rec.data_offset,
                             rec_backup.buf + rec_backup.data_offset,
                             rec.data_len ) == 0 );
    }

exit:

    /* Cleanup */
    mbedtls_ssl_free( &ssl );
    mbedtls_ssl_transform_free( &t0 );
    mbedtls_ssl_transform_free( &t1 );

    mbedtls_free( buf );
}

void test_ssl_crypt_record_wrapper( void ** params )
{

    test_ssl_crypt_record( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ), *( (int *) params[5] ), *( (int *) params[6] ) );
}
void test_ssl_crypt_record_small( int cipher_type, int hash_id,
                             int etm, int tag_mode, int ver,
                             int cid0_len, int cid1_len )
{
    /*
     * Test pairs of encryption and decryption with an increasing
     * amount of space in the record buffer - in more detail:
     * 1) Try to encrypt with 0, 1, 2, ... bytes available
     *    in front of the plaintext, and expect the encryption
     *    to succeed starting from some offset. Always keep
     *    enough space in the end of the buffer.
     * 2) Try to encrypt with 0, 1, 2, ... bytes available
     *    at the end of the plaintext, and expect the encryption
     *    to succeed starting from some offset. Always keep
     *    enough space at the beginning of the buffer.
     * 3) Try to encrypt with 0, 1, 2, ... bytes available
     *    both at the front and end of the plaintext,
     *    and expect the encryption to succeed starting from
     *    some offset.
     *
     * If encryption succeeds, check that decryption succeeds
     * and yields the original record.
     */

    mbedtls_ssl_context ssl; /* ONLY for debugging */

    mbedtls_ssl_transform t0, t1;
    unsigned char *buf = NULL;
    size_t const buflen = 256;
    mbedtls_record rec, rec_backup;

    int ret;
    int mode;              /* Mode 1, 2 or 3 as explained above     */
    size_t offset;         /* Available space at beginning/end/both */
    size_t threshold = 96; /* Maximum offset to test against        */

    size_t default_pre_padding  = 64;  /* Pre-padding to use in mode 2  */
    size_t default_post_padding = 128; /* Post-padding to use in mode 1 */

    int seen_success; /* Indicates if in the current mode we've
                       * already seen a successful test. */

    mbedtls_ssl_init( &ssl );
    mbedtls_ssl_transform_init( &t0 );
    mbedtls_ssl_transform_init( &t1 );
    TEST_ASSERT( build_transforms( &t0, &t1, cipher_type, hash_id,
                                   etm, tag_mode, ver,
                                   (size_t) cid0_len,
                                   (size_t) cid1_len ) == 0 );

    TEST_ASSERT( ( buf = mbedtls_calloc( 1, buflen ) ) != NULL );

    for( mode=1; mode <= 3; mode++ )
    {
        seen_success = 0;
        for( offset=0; offset <= threshold; offset++ )
        {
            mbedtls_ssl_transform *t_dec, *t_enc;
            t_dec = &t0;
            t_enc = &t1;

            memset( rec.ctr, offset, sizeof( rec.ctr ) );
            rec.type    = 42;
            rec.ver[0]  = offset;
            rec.ver[1]  = offset;
            rec.buf     = buf;
            rec.buf_len = buflen;
#if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
            rec.cid_len = 0;
#endif /* MBEDTLS_SSL_DTLS_CONNECTION_ID */

            switch( mode )
            {
                case 1: /* Space in the beginning */
                    rec.data_offset = offset;
                    rec.data_len = buflen - offset - default_post_padding;
                    break;

                case 2: /* Space in the end */
                    rec.data_offset = default_pre_padding;
                    rec.data_len = buflen - default_pre_padding - offset;
                    break;

                case 3: /* Space in the beginning and end */
                    rec.data_offset = offset;
                    rec.data_len = buflen - 2 * offset;
                    break;

                default:
                    TEST_ASSERT( 0 );
                    break;
            }

            memset( rec.buf + rec.data_offset, 42, rec.data_len );

            /* Make a copy for later comparison */
            rec_backup = rec;

            /* Encrypt record */
            ret = mbedtls_ssl_encrypt_buf( &ssl, t_enc, &rec,
                                           mbedtls_test_rnd_std_rand, NULL );

            if( ( mode == 1 || mode == 2 ) && seen_success )
            {
                TEST_ASSERT( ret == 0 );
            }
            else
            {
                TEST_ASSERT( ret == 0 || ret == MBEDTLS_ERR_SSL_BUFFER_TOO_SMALL );
                if( ret == 0 )
                    seen_success = 1;
            }

            if( ret != 0 )
                continue;

#if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
            if( rec.cid_len != 0 )
            {
                /* DTLS 1.2 + CID hides the real content type and
                 * uses a special CID content type in the protected
                 * record. Double-check this. */
                TEST_ASSERT( rec.type == MBEDTLS_SSL_MSG_CID );
            }
#endif /* MBEDTLS_SSL_DTLS_CONNECTION_ID */

#if defined(MBEDTLS_SSL_PROTO_TLS1_3_EXPERIMENTAL)
            if( t_enc->minor_ver == MBEDTLS_SSL_MINOR_VERSION_4 )
            {
                /* TLS 1.3 hides the real content type and
                 * always uses Application Data as the content type
                 * for protected records. Double-check this. */
                TEST_ASSERT( rec.type == MBEDTLS_SSL_MSG_APPLICATION_DATA );
            }
#endif /* MBEDTLS_SSL_PROTO_TLS1_3_EXPERIMENTAL */

            /* Decrypt record with t_dec */
            TEST_EQUAL( 0, mbedtls_ssl_decrypt_buf( &ssl, t_dec, &rec ) );

            /* Compare results */
            TEST_ASSERT( rec.type == rec_backup.type );
            TEST_ASSERT( memcmp( rec.ctr, rec_backup.ctr, 8 ) == 0 );
            TEST_ASSERT( rec.ver[0] == rec_backup.ver[0] );
            TEST_ASSERT( rec.ver[1] == rec_backup.ver[1] );
            TEST_ASSERT( rec.data_len == rec_backup.data_len );
            TEST_ASSERT( rec.data_offset == rec_backup.data_offset );
            TEST_ASSERT( memcmp( rec.buf + rec.data_offset,
                                 rec_backup.buf + rec_backup.data_offset,
                                 rec.data_len ) == 0 );
        }

        TEST_ASSERT( seen_success == 1 );
    }

exit:

    /* Cleanup */
    mbedtls_ssl_free( &ssl );
    mbedtls_ssl_transform_free( &t0 );
    mbedtls_ssl_transform_free( &t1 );

    mbedtls_free( buf );
}

void test_ssl_crypt_record_small_wrapper( void ** params )
{

    test_ssl_crypt_record_small( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ), *( (int *) params[5] ), *( (int *) params[6] ) );
}
#if defined(MBEDTLS_CIPHER_MODE_CBC)
#if defined(MBEDTLS_AES_C)
#if defined(MBEDTLS_SSL_PROTO_TLS1_2)
void test_ssl_decrypt_non_etm_cbc( int cipher_type, int hash_id, int trunc_hmac,
                                   int length_selector )
{
    /*
     * Test record decryption for CBC without EtM, focused on the verification
     * of padding and MAC.
     *
     * Actually depends on TLS >= 1.0 (SSL 3.0 computes the MAC differently),
     * and either AES, ARIA, Camellia or DES, but since the test framework
     * doesn't support alternation in dependency statements, just depend on
     * TLS 1.2 and AES.
     *
     * The length_selector argument is interpreted as follows:
     * - if it's -1, the plaintext length is 0 and minimal padding is applied
     * - if it's -2, the plaintext length is 0 and maximal padding is applied
     * - otherwise it must be in [0, 255] and is padding_length from RFC 5246:
     *   it's the length of the rest of the padding, that is, excluding the
     *   byte that encodes the length. The minimal non-zero plaintext length
     *   that gives this padding_length is automatically selected.
     */
    mbedtls_ssl_context ssl; /* ONLY for debugging */
    mbedtls_ssl_transform t0, t1;
    mbedtls_record rec, rec_save;
    unsigned char *buf = NULL, *buf_save = NULL;
    size_t buflen, olen = 0;
    size_t plaintext_len, block_size, i;
    unsigned char padlen; /* excluding the padding_length byte */
    unsigned char add_data[13];
    unsigned char mac[MBEDTLS_MD_MAX_SIZE];
    int exp_ret;
    const unsigned char pad_max_len = 255; /* Per the standard */

    mbedtls_ssl_init( &ssl );
    mbedtls_ssl_transform_init( &t0 );
    mbedtls_ssl_transform_init( &t1 );

    /* Set up transforms with dummy keys */
    TEST_ASSERT( build_transforms( &t0, &t1, cipher_type, hash_id,
                                   0, trunc_hmac,
                                   MBEDTLS_SSL_MINOR_VERSION_3,
                                   0 , 0 ) == 0 );

    /* Determine padding/plaintext length */
    TEST_ASSERT( length_selector >= -2 && length_selector <= 255 );
    block_size = t0.ivlen;
    if( length_selector < 0 )
    {
        plaintext_len = 0;

        /* Minimal padding
         * The +1 is for the padding_length byte, not counted in padlen. */
        padlen = block_size - ( t0.maclen + 1 ) % block_size;

        /* Maximal padding? */
        if( length_selector == -2 )
            padlen += block_size * ( ( pad_max_len - padlen ) / block_size );
    }
    else
    {
        padlen = length_selector;

        /* Minimal non-zero plaintext_length giving desired padding.
         * The +1 is for the padding_length byte, not counted in padlen. */
        plaintext_len = block_size - ( padlen + t0.maclen + 1 ) % block_size;
    }

    /* Prepare a buffer for record data */
    buflen = block_size
           + plaintext_len
           + t0.maclen
           + padlen + 1;
    ASSERT_ALLOC( buf, buflen );
    ASSERT_ALLOC( buf_save, buflen );

    /* Prepare a dummy record header */
    memset( rec.ctr, 0, sizeof( rec.ctr ) );
    rec.type    = MBEDTLS_SSL_MSG_APPLICATION_DATA;
    rec.ver[0]  = MBEDTLS_SSL_MAJOR_VERSION_3;
    rec.ver[1]  = MBEDTLS_SSL_MINOR_VERSION_3;
#if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
    rec.cid_len = 0;
#endif /* MBEDTLS_SSL_DTLS_CONNECTION_ID */

    /* Prepare dummy record content */
    rec.buf     = buf;
    rec.buf_len = buflen;
    rec.data_offset = block_size;
    rec.data_len = plaintext_len;
    memset( rec.buf + rec.data_offset, 42, rec.data_len );

    /* Serialized version of record header for MAC purposes */
    memcpy( add_data, rec.ctr, 8 );
    add_data[8] = rec.type;
    add_data[9] = rec.ver[0];
    add_data[10] = rec.ver[1];
    add_data[11] = ( rec.data_len >> 8 ) & 0xff;
    add_data[12] = ( rec.data_len >> 0 ) & 0xff;

    /* Set dummy IV */
    memset( t0.iv_enc, 0x55, t0.ivlen );
    memcpy( rec.buf, t0.iv_enc, t0.ivlen );

    /*
     * Prepare a pre-encryption record (with MAC and padding), and save it.
     */

    /* MAC with additional data */
    TEST_EQUAL( 0, mbedtls_md_hmac_update( &t0.md_ctx_enc, add_data, 13 ) );
    TEST_EQUAL( 0, mbedtls_md_hmac_update( &t0.md_ctx_enc,
                                           rec.buf + rec.data_offset,
                                           rec.data_len ) );
    TEST_EQUAL( 0, mbedtls_md_hmac_finish( &t0.md_ctx_enc, mac ) );

    memcpy( rec.buf + rec.data_offset + rec.data_len, mac, t0.maclen );
    rec.data_len += t0.maclen;

    /* Pad */
    memset( rec.buf + rec.data_offset + rec.data_len, padlen, padlen + 1 );
    rec.data_len += padlen + 1;

    /* Save correct pre-encryption record */
    rec_save = rec;
    rec_save.buf = buf_save;
    memcpy( buf_save, buf, buflen );

    /*
     * Encrypt and decrypt the correct record, expecting success
     */
    TEST_EQUAL( 0, mbedtls_cipher_crypt( &t0.cipher_ctx_enc,
                                  t0.iv_enc, t0.ivlen,
                                  rec.buf + rec.data_offset, rec.data_len,
                                  rec.buf + rec.data_offset, &olen ) );
    rec.data_offset -= t0.ivlen;
    rec.data_len    += t0.ivlen;

    TEST_EQUAL( 0, mbedtls_ssl_decrypt_buf( &ssl, &t1, &rec ) );

    /*
     * Modify each byte of the pre-encryption record before encrypting and
     * decrypting it, expecting failure every time.
     *
     * We use RANDOMMNESS because this loop runs hundreds of times and this
     * function runs hundreds of times. So it can very easily contribute to
     * hundreds of milliseconds of latency, which we can't have in our pure
     * testing infrastructure.
     */
    for( i = block_size; i < buflen; i += max( 1, _rand64() & 31 ) )
    {
        mbedtls_test_set_step( i );
        /* Restore correct pre-encryption record */
        rec = rec_save;
        rec.buf = buf;
        memcpy( buf, buf_save, buflen );
        /* Corrupt one byte of the data (could be plaintext, MAC or padding) */
        rec.buf[i] ^= 0x01;
        /* Encrypt */
        TEST_EQUAL( 0, mbedtls_cipher_crypt( &t0.cipher_ctx_enc,
                                      t0.iv_enc, t0.ivlen,
                                      rec.buf + rec.data_offset, rec.data_len,
                                      rec.buf + rec.data_offset, &olen ) );
        rec.data_offset -= t0.ivlen;
        rec.data_len    += t0.ivlen;
        /* Decrypt and expect failure */
        TEST_EQUAL( MBEDTLS_ERR_SSL_INVALID_MAC,
                    mbedtls_ssl_decrypt_buf( &ssl, &t1, &rec ) );
    }

    /*
     * Use larger values of the padding bytes - with small buffers, this tests
     * the case where the announced padlen would be larger than the buffer
     * (and before that, than the buffer minus the size of the MAC), to make
     * sure our padding checking code does not perform any out-of-bounds reads
     * in this case. (With larger buffers, ie when the plaintext is long or
     * maximal length padding is used, this is less relevant but still doesn't
     * hurt to test.)
     *
     * (Start the loop with correct padding, just to double-check that record
     * saving did work, and that we're overwriting the correct bytes.)
     *
     * We use RANDOMMNESS because this loop runs hundreds of times and this
     * function runs hundreds of times. So it can very easily contribute to
     * hundreds of milliseconds of latency, which we can't have in our pure
     * testing infrastructure.
     */
    for( i = padlen; i <= pad_max_len; i += max( 1, _rand64() & 31 ) )
    {
        mbedtls_test_set_step( i );
        /* Restore correct pre-encryption record */
        rec = rec_save;
        rec.buf = buf;
        memcpy( buf, buf_save, buflen );
        /* Set padding bytes to new value */
        memset( buf + buflen - padlen - 1, i, padlen + 1 );
        /* Encrypt */
        TEST_EQUAL( 0, mbedtls_cipher_crypt( &t0.cipher_ctx_enc,
                                      t0.iv_enc, t0.ivlen,
                                      rec.buf + rec.data_offset, rec.data_len,
                                      rec.buf + rec.data_offset, &olen ) );
        rec.data_offset -= t0.ivlen;
        rec.data_len    += t0.ivlen;
        /* Decrypt and expect failure except the first time */
        exp_ret = ( i == padlen ) ? 0 : MBEDTLS_ERR_SSL_INVALID_MAC;
        TEST_EQUAL( exp_ret, mbedtls_ssl_decrypt_buf( &ssl, &t1, &rec ) );
    }

exit:
    mbedtls_ssl_free( &ssl );
    mbedtls_ssl_transform_free( &t0 );
    mbedtls_ssl_transform_free( &t1 );
    mbedtls_free( buf );
    mbedtls_free( buf_save );
}

void test_ssl_decrypt_non_etm_cbc_wrapper( void ** params )
{

    test_ssl_decrypt_non_etm_cbc( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ) );
}
#endif /* MBEDTLS_SSL_PROTO_TLS1_2 */
#endif /* MBEDTLS_AES_C */
#endif /* MBEDTLS_CIPHER_MODE_CBC */
#if defined(MBEDTLS_SSL_PROTO_TLS1_3_EXPERIMENTAL)
void test_ssl_tls1_3_hkdf_expand_label( int hash_alg,
                                   data_t *secret,
                                   int label_idx,
                                   data_t *ctx,
                                   int desired_length,
                                   data_t *expected )
{
    unsigned char dst[ 100 ];

    unsigned char const *lbl = NULL;
    size_t lbl_len;
#define MBEDTLS_SSL_TLS1_3_LABEL( name, string )                        \
    if( label_idx == (int) tls1_3_label_ ## name )                      \
    {                                                                   \
        lbl = mbedtls_ssl_tls1_3_labels.name;                           \
        lbl_len = sizeof( mbedtls_ssl_tls1_3_labels.name );             \
    }
MBEDTLS_SSL_TLS1_3_LABEL_LIST
#undef MBEDTLS_SSL_TLS1_3_LABEL
    TEST_ASSERT( lbl != NULL );

    /* Check sanity of test parameters. */
    TEST_ASSERT( (size_t) desired_length <= sizeof(dst) );
    TEST_ASSERT( (size_t) desired_length == expected->len );

    TEST_ASSERT( mbedtls_ssl_tls1_3_hkdf_expand_label(
                      (mbedtls_md_type_t) hash_alg,
                      secret->x, secret->len,
                      lbl, lbl_len,
                      ctx->x, ctx->len,
                      dst, desired_length ) == 0 );

    ASSERT_COMPARE( dst, (size_t) desired_length,
                    expected->x, (size_t) expected->len );
exit:
    ;
}

void test_ssl_tls1_3_hkdf_expand_label_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data7 = {(uint8_t *) params[7], *( (uint32_t *) params[8] )};

    test_ssl_tls1_3_hkdf_expand_label( *( (int *) params[0] ), &data1, *( (int *) params[3] ), &data4, *( (int *) params[6] ), &data7 );
}
#endif /* MBEDTLS_SSL_PROTO_TLS1_3_EXPERIMENTAL */
#if defined(MBEDTLS_SSL_PROTO_TLS1_3_EXPERIMENTAL)
void test_ssl_tls1_3_traffic_key_generation( int hash_alg,
                                        data_t *server_secret,
                                        data_t *client_secret,
                                        int desired_iv_len,
                                        int desired_key_len,
                                        data_t *expected_server_write_key,
                                        data_t *expected_server_write_iv,
                                        data_t *expected_client_write_key,
                                        data_t *expected_client_write_iv )
{
    mbedtls_ssl_key_set keys;

    /* Check sanity of test parameters. */
    TEST_ASSERT( client_secret->len == server_secret->len );
    TEST_ASSERT( expected_client_write_iv->len == expected_server_write_iv->len &&
                 expected_client_write_iv->len == (size_t) desired_iv_len );
    TEST_ASSERT( expected_client_write_key->len == expected_server_write_key->len &&
                 expected_client_write_key->len == (size_t) desired_key_len );

    TEST_ASSERT( mbedtls_ssl_tls1_3_make_traffic_keys(
                     (mbedtls_md_type_t) hash_alg,
                     client_secret->x,
                     server_secret->x,
                     client_secret->len /* == server_secret->len */,
                     desired_key_len, desired_iv_len,
                     &keys ) == 0 );

    ASSERT_COMPARE( keys.client_write_key,
                    keys.key_len,
                    expected_client_write_key->x,
                    (size_t) desired_key_len );
    ASSERT_COMPARE( keys.server_write_key,
                    keys.key_len,
                    expected_server_write_key->x,
                    (size_t) desired_key_len );
    ASSERT_COMPARE( keys.client_write_iv,
                    keys.iv_len,
                    expected_client_write_iv->x,
                    (size_t) desired_iv_len );
    ASSERT_COMPARE( keys.server_write_iv,
                    keys.iv_len,
                    expected_server_write_iv->x,
                    (size_t) desired_iv_len );
exit:
    ;
}

void test_ssl_tls1_3_traffic_key_generation_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data3 = {(uint8_t *) params[3], *( (uint32_t *) params[4] )};
    data_t data7 = {(uint8_t *) params[7], *( (uint32_t *) params[8] )};
    data_t data9 = {(uint8_t *) params[9], *( (uint32_t *) params[10] )};
    data_t data11 = {(uint8_t *) params[11], *( (uint32_t *) params[12] )};
    data_t data13 = {(uint8_t *) params[13], *( (uint32_t *) params[14] )};

    test_ssl_tls1_3_traffic_key_generation( *( (int *) params[0] ), &data1, &data3, *( (int *) params[5] ), *( (int *) params[6] ), &data7, &data9, &data11, &data13 );
}
#endif /* MBEDTLS_SSL_PROTO_TLS1_3_EXPERIMENTAL */
#if defined(MBEDTLS_SSL_PROTO_TLS1_3_EXPERIMENTAL)
void test_ssl_tls1_3_derive_secret( int hash_alg,
                               data_t *secret,
                               int label_idx,
                               data_t *ctx,
                               int desired_length,
                               int already_hashed,
                               data_t *expected )
{
    unsigned char dst[ 100 ];

    unsigned char const *lbl = NULL;
    size_t lbl_len;
#define MBEDTLS_SSL_TLS1_3_LABEL( name, string )                        \
    if( label_idx == (int) tls1_3_label_ ## name )                      \
    {                                                                   \
        lbl = mbedtls_ssl_tls1_3_labels.name;                           \
        lbl_len = sizeof( mbedtls_ssl_tls1_3_labels.name );             \
    }
MBEDTLS_SSL_TLS1_3_LABEL_LIST
#undef MBEDTLS_SSL_TLS1_3_LABEL
    TEST_ASSERT( lbl != NULL );

    /* Check sanity of test parameters. */
    TEST_ASSERT( (size_t) desired_length <= sizeof(dst) );
    TEST_ASSERT( (size_t) desired_length == expected->len );

    TEST_ASSERT( mbedtls_ssl_tls1_3_derive_secret(
                      (mbedtls_md_type_t) hash_alg,
                      secret->x, secret->len,
                      lbl, lbl_len,
                      ctx->x, ctx->len,
                      already_hashed,
                      dst, desired_length ) == 0 );

    ASSERT_COMPARE( dst, desired_length,
                    expected->x, desired_length );
exit:
    ;
}

void test_ssl_tls1_3_derive_secret_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data8 = {(uint8_t *) params[8], *( (uint32_t *) params[9] )};

    test_ssl_tls1_3_derive_secret( *( (int *) params[0] ), &data1, *( (int *) params[3] ), &data4, *( (int *) params[6] ), *( (int *) params[7] ), &data8 );
}
#endif /* MBEDTLS_SSL_PROTO_TLS1_3_EXPERIMENTAL */
#if defined(MBEDTLS_SSL_PROTO_TLS1_3_EXPERIMENTAL)
void test_ssl_tls1_3_key_evolution( int hash_alg,
                               data_t *secret,
                               data_t *input,
                               data_t *expected )
{
    unsigned char secret_new[ MBEDTLS_MD_MAX_SIZE ];

    TEST_ASSERT( mbedtls_ssl_tls1_3_evolve_secret(
                      (mbedtls_md_type_t) hash_alg,
                      secret->len ? secret->x : NULL,
                      input->len ? input->x : NULL, input->len,
                      secret_new ) == 0 );

    ASSERT_COMPARE( secret_new, (size_t) expected->len,
                    expected->x, (size_t) expected->len );
exit:
    ;
}

void test_ssl_tls1_3_key_evolution_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data3 = {(uint8_t *) params[3], *( (uint32_t *) params[4] )};
    data_t data5 = {(uint8_t *) params[5], *( (uint32_t *) params[6] )};

    test_ssl_tls1_3_key_evolution( *( (int *) params[0] ), &data1, &data3, &data5 );
}
#endif /* MBEDTLS_SSL_PROTO_TLS1_3_EXPERIMENTAL */
void test_ssl_tls_prf( int type, data_t * secret, data_t * random,
                  char *label, data_t *result_str, int exp_ret )
{
    unsigned char *output;

    output = mbedtls_calloc( 1, result_str->len );
    if( output == NULL )
        goto exit;

    USE_PSA_INIT( );

    TEST_ASSERT( mbedtls_ssl_tls_prf( type, secret->x, secret->len,
                                      label, random->x, random->len,
                                      output, result_str->len ) == exp_ret );

    if( exp_ret == 0 )
    {
        TEST_ASSERT( mbedtls_test_hexcmp( output, result_str->x,
                     result_str->len, result_str->len ) == 0 );
    }
exit:

    mbedtls_free( output );
    USE_PSA_DONE( );
}

void test_ssl_tls_prf_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data3 = {(uint8_t *) params[3], *( (uint32_t *) params[4] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};

    test_ssl_tls_prf( *( (int *) params[0] ), &data1, &data3, (char *) params[5], &data6, *( (int *) params[8] ) );
}
void test_ssl_serialize_session_save_load( int ticket_len, char *crt_file )
{
    mbedtls_ssl_session original, restored;
    unsigned char *buf = NULL;
    size_t len;

    /*
     * Test that a save-load pair is the identity
     */

    mbedtls_ssl_session_init( &original );
    mbedtls_ssl_session_init( &restored );

    /* Prepare a dummy session to work on */
    TEST_ASSERT( ssl_populate_session( &original, ticket_len, crt_file ) == 0 );

    /* Serialize it */
    TEST_ASSERT( mbedtls_ssl_session_save( &original, NULL, 0, &len )
                 == MBEDTLS_ERR_SSL_BUFFER_TOO_SMALL );
    TEST_ASSERT( ( buf = mbedtls_calloc( 1, len ) ) != NULL );
    TEST_ASSERT( mbedtls_ssl_session_save( &original, buf, len, &len )
                 == 0 );

    /* Restore session from serialized data */
    TEST_ASSERT( mbedtls_ssl_session_load( &restored, buf, len) == 0 );

    /*
     * Make sure both session structures are identical
     */
#if defined(MBEDTLS_HAVE_TIME)
    TEST_ASSERT( original.start == restored.start );
#endif
    TEST_ASSERT( original.ciphersuite == restored.ciphersuite );
    TEST_ASSERT( original.compression == restored.compression );
    TEST_ASSERT( original.id_len == restored.id_len );
    TEST_ASSERT( memcmp( original.id,
                         restored.id, sizeof( original.id ) ) == 0 );
    TEST_ASSERT( memcmp( original.master,
                         restored.master, sizeof( original.master ) ) == 0 );

#if defined(MBEDTLS_X509_CRT_PARSE_C)
#if defined(MBEDTLS_SSL_KEEP_PEER_CERTIFICATE)
    TEST_ASSERT( ( original.peer_cert == NULL ) ==
                 ( restored.peer_cert == NULL ) );
    if( original.peer_cert != NULL )
    {
        TEST_ASSERT( original.peer_cert->raw.len ==
                     restored.peer_cert->raw.len );
        TEST_ASSERT( memcmp( original.peer_cert->raw.p,
                             restored.peer_cert->raw.p,
                             original.peer_cert->raw.len ) == 0 );
    }
#else /* MBEDTLS_SSL_KEEP_PEER_CERTIFICATE */
    TEST_ASSERT( original.peer_cert_digest_type ==
                 restored.peer_cert_digest_type );
    TEST_ASSERT( original.peer_cert_digest_len ==
                 restored.peer_cert_digest_len );
    TEST_ASSERT( ( original.peer_cert_digest == NULL ) ==
                 ( restored.peer_cert_digest == NULL ) );
    if( original.peer_cert_digest != NULL )
    {
        TEST_ASSERT( memcmp( original.peer_cert_digest,
                             restored.peer_cert_digest,
                             original.peer_cert_digest_len ) == 0 );
    }
#endif /* MBEDTLS_SSL_KEEP_PEER_CERTIFICATE */
#endif /* MBEDTLS_X509_CRT_PARSE_C */
    TEST_ASSERT( original.verify_result == restored.verify_result );

#if defined(MBEDTLS_SSL_SESSION_TICKETS) && defined(MBEDTLS_SSL_CLI_C)
    TEST_ASSERT( original.ticket_len == restored.ticket_len );
    if( original.ticket_len != 0 )
    {
        TEST_ASSERT( original.ticket != NULL );
        TEST_ASSERT( restored.ticket != NULL );
        TEST_ASSERT( memcmp( original.ticket,
                             restored.ticket, original.ticket_len ) == 0 );
    }
    TEST_ASSERT( original.ticket_lifetime == restored.ticket_lifetime );
#endif

#if defined(MBEDTLS_SSL_MAX_FRAGMENT_LENGTH)
    TEST_ASSERT( original.mfl_code == restored.mfl_code );
#endif

#if defined(MBEDTLS_SSL_ENCRYPT_THEN_MAC)
    TEST_ASSERT( original.encrypt_then_mac == restored.encrypt_then_mac );
#endif

exit:
    mbedtls_ssl_session_free( &original );
    mbedtls_ssl_session_free( &restored );
    mbedtls_free( buf );
}

void test_ssl_serialize_session_save_load_wrapper( void ** params )
{

    test_ssl_serialize_session_save_load( *( (int *) params[0] ), (char *) params[1] );
}
void test_ssl_serialize_session_load_save( int ticket_len, char *crt_file )
{
    mbedtls_ssl_session session;
    unsigned char *buf1 = NULL, *buf2 = NULL;
    size_t len0, len1, len2;

    /*
     * Test that a load-save pair is the identity
     */

    mbedtls_ssl_session_init( &session );

    /* Prepare a dummy session to work on */
    TEST_ASSERT( ssl_populate_session( &session, ticket_len, crt_file ) == 0 );

    /* Get desired buffer size for serializing */
    TEST_ASSERT( mbedtls_ssl_session_save( &session, NULL, 0, &len0 )
                 == MBEDTLS_ERR_SSL_BUFFER_TOO_SMALL );

    /* Allocate first buffer */
    buf1 = mbedtls_calloc( 1, len0 );
    TEST_ASSERT( buf1 != NULL );

    /* Serialize to buffer and free live session */
    TEST_ASSERT( mbedtls_ssl_session_save( &session, buf1, len0, &len1 )
                 == 0 );
    TEST_ASSERT( len0 == len1 );
    mbedtls_ssl_session_free( &session );

    /* Restore session from serialized data */
    TEST_ASSERT( mbedtls_ssl_session_load( &session, buf1, len1 ) == 0 );

    /* Allocate second buffer and serialize to it */
    buf2 = mbedtls_calloc( 1, len0 );
    TEST_ASSERT( buf2 != NULL );
    TEST_ASSERT( mbedtls_ssl_session_save( &session, buf2, len0, &len2 )
                 == 0 );

    /* Make sure both serialized versions are identical */
    TEST_ASSERT( len1 == len2 );
    TEST_ASSERT( memcmp( buf1, buf2, len1 ) == 0 );

exit:
    mbedtls_ssl_session_free( &session );
    mbedtls_free( buf1 );
    mbedtls_free( buf2 );
}

void test_ssl_serialize_session_load_save_wrapper( void ** params )
{

    test_ssl_serialize_session_load_save( *( (int *) params[0] ), (char *) params[1] );
}
void test_ssl_serialize_session_save_buf_size( int ticket_len, char *crt_file )
{
    mbedtls_ssl_session session;
    unsigned char *buf = NULL;
    size_t good_len, bad_len, test_len;

    /*
     * Test that session_save() fails cleanly on small buffers
     */

    mbedtls_ssl_session_init( &session );

    /* Prepare dummy session and get serialized size */
    TEST_ASSERT( ssl_populate_session( &session, ticket_len, crt_file ) == 0 );
    TEST_ASSERT( mbedtls_ssl_session_save( &session, NULL, 0, &good_len )
                 == MBEDTLS_ERR_SSL_BUFFER_TOO_SMALL );

    /* Try all possible bad lengths */
    for( bad_len = 1; bad_len < good_len; bad_len++ )
    {
        /* Allocate exact size so that asan/valgrind can detect any overwrite */
        mbedtls_free( buf );
        TEST_ASSERT( ( buf = mbedtls_calloc( 1, bad_len ) ) != NULL );
        TEST_ASSERT( mbedtls_ssl_session_save( &session, buf, bad_len,
                                               &test_len )
                     == MBEDTLS_ERR_SSL_BUFFER_TOO_SMALL );
        TEST_ASSERT( test_len == good_len );
    }

exit:
    mbedtls_ssl_session_free( &session );
    mbedtls_free( buf );
}

void test_ssl_serialize_session_save_buf_size_wrapper( void ** params )
{

    test_ssl_serialize_session_save_buf_size( *( (int *) params[0] ), (char *) params[1] );
}
void test_ssl_serialize_session_load_buf_size( int ticket_len, char *crt_file )
{
    mbedtls_ssl_session session;
    unsigned char *good_buf = NULL, *bad_buf = NULL;
    size_t good_len, bad_len;

    /*
     * Test that session_load() fails cleanly on small buffers
     */

    mbedtls_ssl_session_init( &session );

    /* Prepare serialized session data */
    TEST_ASSERT( ssl_populate_session( &session, ticket_len, crt_file ) == 0 );
    TEST_ASSERT( mbedtls_ssl_session_save( &session, NULL, 0, &good_len )
                 == MBEDTLS_ERR_SSL_BUFFER_TOO_SMALL );
    TEST_ASSERT( ( good_buf = mbedtls_calloc( 1, good_len ) ) != NULL );
    TEST_ASSERT( mbedtls_ssl_session_save( &session, good_buf, good_len,
                                           &good_len ) == 0 );
    mbedtls_ssl_session_free( &session );

    /* Try all possible bad lengths */
    for( bad_len = 0; bad_len < good_len; bad_len++ )
    {
        /* Allocate exact size so that asan/valgrind can detect any overread */
        mbedtls_free( bad_buf );
        bad_buf = mbedtls_calloc( 1, bad_len ? bad_len : 1 );
        TEST_ASSERT( bad_buf != NULL );
        memcpy( bad_buf, good_buf, bad_len );

        TEST_ASSERT( mbedtls_ssl_session_load( &session, bad_buf, bad_len )
                     == MBEDTLS_ERR_SSL_BAD_INPUT_DATA );
    }

exit:
    mbedtls_ssl_session_free( &session );
    mbedtls_free( good_buf );
    mbedtls_free( bad_buf );
}

void test_ssl_serialize_session_load_buf_size_wrapper( void ** params )
{

    test_ssl_serialize_session_load_buf_size( *( (int *) params[0] ), (char *) params[1] );
}
void test_ssl_session_serialize_version_check( int corrupt_major,
                                          int corrupt_minor,
                                          int corrupt_patch,
                                          int corrupt_config )
{
    unsigned char serialized_session[ 2048 ];
    size_t serialized_session_len;
    unsigned cur_byte;
    mbedtls_ssl_session session;
    uint8_t should_corrupt_byte[] = { corrupt_major  == 1,
                                      corrupt_minor  == 1,
                                      corrupt_patch  == 1,
                                      corrupt_config == 1,
                                      corrupt_config == 1 };

    mbedtls_ssl_session_init( &session );

    /* Infer length of serialized session. */
    TEST_ASSERT( mbedtls_ssl_session_save( &session,
                                           serialized_session,
                                           sizeof( serialized_session ),
                                           &serialized_session_len ) == 0 );

    mbedtls_ssl_session_free( &session );

    /* Without any modification, we should be able to successfully
     * de-serialize the session - double-check that. */
    TEST_ASSERT( mbedtls_ssl_session_load( &session,
                                           serialized_session,
                                           serialized_session_len ) == 0 );
    mbedtls_ssl_session_free( &session );

    /* Go through the bytes in the serialized session header and
     * corrupt them bit-by-bit. */
    for( cur_byte = 0; cur_byte < sizeof( should_corrupt_byte ); cur_byte++ )
    {
        int cur_bit;
        unsigned char * const byte = &serialized_session[ cur_byte ];

        if( should_corrupt_byte[ cur_byte ] == 0 )
            continue;

        for( cur_bit = 0; cur_bit < CHAR_BIT; cur_bit++ )
        {
            unsigned char const corrupted_bit = 0x1u << cur_bit;
            /* Modify a single bit in the serialized session. */
            *byte ^= corrupted_bit;

            /* Attempt to deserialize */
            TEST_ASSERT( mbedtls_ssl_session_load( &session,
                                                   serialized_session,
                                                   serialized_session_len ) ==
                         MBEDTLS_ERR_SSL_VERSION_MISMATCH );

            /* Undo the change */
            *byte ^= corrupted_bit;
        }
    }

exit:
    ;
}

void test_ssl_session_serialize_version_check_wrapper( void ** params )
{

    test_ssl_session_serialize_version_check( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ) );
}
#if defined(MBEDTLS_X509_CRT_PARSE_C)
#if defined(MBEDTLS_RSA_C)
#if defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED)
#if !defined(MBEDTLS_USE_PSA_CRYPTO)
#if defined(MBEDTLS_PKCS1_V15)
#if defined(MBEDTLS_ENTROPY_C)
#if defined(MBEDTLS_ENTROPY_C)
#if defined(MBEDTLS_CTR_DRBG_C)
void test_mbedtls_endpoint_sanity( int endpoint_type )
{
    enum { BUFFSIZE = 1024 };
    mbedtls_endpoint ep;
    int ret = -1;

    ret = mbedtls_endpoint_init( NULL, endpoint_type, MBEDTLS_PK_RSA,
                                 NULL, NULL, NULL );
    TEST_ASSERT( MBEDTLS_ERR_SSL_BAD_INPUT_DATA == ret );

    ret = mbedtls_endpoint_certificate_init( NULL, MBEDTLS_PK_RSA );
    TEST_ASSERT( MBEDTLS_ERR_SSL_BAD_INPUT_DATA == ret );

    ret = mbedtls_endpoint_init( &ep, endpoint_type, MBEDTLS_PK_RSA,
                                 NULL, NULL, NULL );
    TEST_ASSERT( ret == 0 );

exit:
    mbedtls_endpoint_free( &ep, NULL );
}

void test_mbedtls_endpoint_sanity_wrapper( void ** params )
{

    test_mbedtls_endpoint_sanity( *( (int *) params[0] ) );
}
#endif /* MBEDTLS_CTR_DRBG_C */
#endif /* MBEDTLS_ENTROPY_C */
#endif /* MBEDTLS_ENTROPY_C */
#endif /* MBEDTLS_PKCS1_V15 */
#endif /* !MBEDTLS_USE_PSA_CRYPTO */
#endif /* MBEDTLS_ECP_DP_SECP384R1_ENABLED */
#endif /* MBEDTLS_RSA_C */
#endif /* MBEDTLS_X509_CRT_PARSE_C */
#if defined(MBEDTLS_X509_CRT_PARSE_C)
#if defined(MBEDTLS_RSA_C)
#if defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED)
#if !defined(MBEDTLS_USE_PSA_CRYPTO)
#if defined(MBEDTLS_PKCS1_V15)
#if defined(MBEDTLS_ENTROPY_C)
#if defined(MBEDTLS_ENTROPY_C)
#if defined(MBEDTLS_CTR_DRBG_C)
void test_move_handshake_to_state(int endpoint_type, int state, int need_pass)
{
    enum { BUFFSIZE = 1024 };
    mbedtls_endpoint base_ep, second_ep;
    int ret = -1;

    ret = mbedtls_endpoint_init( &base_ep, endpoint_type, MBEDTLS_PK_RSA,
                                 NULL, NULL, NULL );
    TEST_ASSERT( ret == 0 );

    ret = mbedtls_endpoint_init( &second_ep,
                            ( endpoint_type == MBEDTLS_SSL_IS_SERVER ) ?
                            MBEDTLS_SSL_IS_CLIENT : MBEDTLS_SSL_IS_SERVER,
                                 MBEDTLS_PK_RSA, NULL, NULL, NULL );
    TEST_ASSERT( ret == 0 );

    ret = mbedtls_mock_socket_connect( &(base_ep.socket_),
                                       &(second_ep.socket_),
                                       BUFFSIZE );
    TEST_ASSERT( ret == 0 );

    ret = mbedtls_move_handshake_to_state( &(base_ep.ssl),
                                           &(second_ep.ssl),
                                           state );
    if( need_pass )
    {
        TEST_ASSERT( ret == 0 );
        TEST_ASSERT( base_ep.ssl.state == state );
    }
    else
    {
        TEST_ASSERT( ret != 0 );
        TEST_ASSERT( base_ep.ssl.state != state );
    }

exit:
    mbedtls_endpoint_free( &base_ep, NULL );
    mbedtls_endpoint_free( &second_ep, NULL );
}

void test_move_handshake_to_state_wrapper( void ** params )
{

    test_move_handshake_to_state( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ) );
}
#endif /* MBEDTLS_CTR_DRBG_C */
#endif /* MBEDTLS_ENTROPY_C */
#endif /* MBEDTLS_ENTROPY_C */
#endif /* MBEDTLS_PKCS1_V15 */
#endif /* !MBEDTLS_USE_PSA_CRYPTO */
#endif /* MBEDTLS_ECP_DP_SECP384R1_ENABLED */
#endif /* MBEDTLS_RSA_C */
#endif /* MBEDTLS_X509_CRT_PARSE_C */
#if defined(MBEDTLS_X509_CRT_PARSE_C)
#if !defined(MBEDTLS_USE_PSA_CRYPTO)
#if defined(MBEDTLS_PKCS1_V15)
#if defined(MBEDTLS_RSA_C)
#if defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED)
#if defined(MBEDTLS_ENTROPY_C)
#if defined(MBEDTLS_CTR_DRBG_C)
void test_handshake_version( int dtls, int client_min_version, int client_max_version,
                        int server_min_version, int server_max_version,
                        int expected_negotiated_version )
{
    handshake_test_options options;
    init_handshake_options( &options );

    options.client_min_version = client_min_version;
    options.client_max_version = client_max_version;
    options.server_min_version = server_min_version;
    options.server_max_version = server_max_version;

    options.expected_negotiated_version = expected_negotiated_version;

    options.dtls = dtls;
    /* By default, SSLv3.0 and TLSv1.0 use 1/n-1 splitting when sending data, so
     * the number of fragments will be twice as big. */
    if( expected_negotiated_version == MBEDTLS_SSL_MINOR_VERSION_0 ||
        expected_negotiated_version == MBEDTLS_SSL_MINOR_VERSION_1 )
    {
        options.expected_cli_fragments = 2;
        options.expected_srv_fragments = 2;
    }
    perform_handshake( &options );

    /* The goto below is used to avoid an "unused label" warning.*/
    goto exit;
exit:
    ;
}

void test_handshake_version_wrapper( void ** params )
{

    test_handshake_version( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ), *( (int *) params[5] ) );
}
#endif /* MBEDTLS_CTR_DRBG_C */
#endif /* MBEDTLS_ENTROPY_C */
#endif /* MBEDTLS_ECP_DP_SECP384R1_ENABLED */
#endif /* MBEDTLS_RSA_C */
#endif /* MBEDTLS_PKCS1_V15 */
#endif /* !MBEDTLS_USE_PSA_CRYPTO */
#endif /* MBEDTLS_X509_CRT_PARSE_C */
#if defined(MBEDTLS_X509_CRT_PARSE_C)
#if !defined(MBEDTLS_USE_PSA_CRYPTO)
#if defined(MBEDTLS_PKCS1_V15)
#if defined(MBEDTLS_SSL_PROTO_TLS1_2)
#if defined(MBEDTLS_ENTROPY_C)
#if defined(MBEDTLS_CTR_DRBG_C)
void test_handshake_psk_cipher( char* cipher, int pk_alg, data_t *psk_str, int dtls )
{
    handshake_test_options options;
    init_handshake_options( &options );

    options.cipher = cipher;
    options.dtls = dtls;
    options.psk_str = psk_str;
    options.pk_alg = pk_alg;

    perform_handshake( &options );

    /* The goto below is used to avoid an "unused label" warning.*/
    goto exit;
exit:
    ;
}

void test_handshake_psk_cipher_wrapper( void ** params )
{
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};

    test_handshake_psk_cipher( (char *) params[0], *( (int *) params[1] ), &data2, *( (int *) params[4] ) );
}
#endif /* MBEDTLS_CTR_DRBG_C */
#endif /* MBEDTLS_ENTROPY_C */
#endif /* MBEDTLS_SSL_PROTO_TLS1_2 */
#endif /* MBEDTLS_PKCS1_V15 */
#endif /* !MBEDTLS_USE_PSA_CRYPTO */
#endif /* MBEDTLS_X509_CRT_PARSE_C */
#if defined(MBEDTLS_X509_CRT_PARSE_C)
#if !defined(MBEDTLS_USE_PSA_CRYPTO)
#if defined(MBEDTLS_PKCS1_V15)
#if defined(MBEDTLS_SSL_PROTO_TLS1_2)
#if defined(MBEDTLS_ENTROPY_C)
#if defined(MBEDTLS_CTR_DRBG_C)
void test_handshake_cipher( char* cipher, int pk_alg, int dtls )
{
    test_handshake_psk_cipher( cipher, pk_alg, NULL, dtls );

    /* The goto below is used to avoid an "unused label" warning.*/
    goto exit;
exit:
    ;
}

void test_handshake_cipher_wrapper( void ** params )
{

    test_handshake_cipher( (char *) params[0], *( (int *) params[1] ), *( (int *) params[2] ) );
}
#endif /* MBEDTLS_CTR_DRBG_C */
#endif /* MBEDTLS_ENTROPY_C */
#endif /* MBEDTLS_SSL_PROTO_TLS1_2 */
#endif /* MBEDTLS_PKCS1_V15 */
#endif /* !MBEDTLS_USE_PSA_CRYPTO */
#endif /* MBEDTLS_X509_CRT_PARSE_C */
#if defined(MBEDTLS_X509_CRT_PARSE_C)
#if !defined(MBEDTLS_USE_PSA_CRYPTO)
#if defined(MBEDTLS_PKCS1_V15)
#if defined(MBEDTLS_SSL_PROTO_TLS1_2)
#if defined(MBEDTLS_RSA_C)
#if defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED)
#if defined(MBEDTLS_ENTROPY_C)
#if defined(MBEDTLS_CTR_DRBG_C)
void test_app_data( int mfl, int cli_msg_len, int srv_msg_len,
               int expected_cli_fragments,
               int expected_srv_fragments, int dtls )
{
    handshake_test_options options;
    init_handshake_options( &options );

    options.mfl = mfl;
    options.cli_msg_len = cli_msg_len;
    options.srv_msg_len = srv_msg_len;
    options.expected_cli_fragments = expected_cli_fragments;
    options.expected_srv_fragments = expected_srv_fragments;
    options.dtls = dtls;

    perform_handshake( &options );
    /* The goto below is used to avoid an "unused label" warning.*/
    goto exit;
exit:
    ;
}

void test_app_data_wrapper( void ** params )
{

    test_app_data( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ), *( (int *) params[5] ) );
}
#endif /* MBEDTLS_CTR_DRBG_C */
#endif /* MBEDTLS_ENTROPY_C */
#endif /* MBEDTLS_ECP_DP_SECP384R1_ENABLED */
#endif /* MBEDTLS_RSA_C */
#endif /* MBEDTLS_SSL_PROTO_TLS1_2 */
#endif /* MBEDTLS_PKCS1_V15 */
#endif /* !MBEDTLS_USE_PSA_CRYPTO */
#endif /* MBEDTLS_X509_CRT_PARSE_C */
#if defined(MBEDTLS_X509_CRT_PARSE_C)
#if !defined(MBEDTLS_USE_PSA_CRYPTO)
#if defined(MBEDTLS_PKCS1_V15)
#if defined(MBEDTLS_SSL_PROTO_TLS1_2)
#if defined(MBEDTLS_RSA_C)
#if defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED)
#if defined(MBEDTLS_ENTROPY_C)
#if defined(MBEDTLS_CTR_DRBG_C)
void test_app_data_tls( int mfl, int cli_msg_len, int srv_msg_len,
                   int expected_cli_fragments,
                   int expected_srv_fragments )
{
    test_app_data( mfl, cli_msg_len, srv_msg_len, expected_cli_fragments,
                   expected_srv_fragments, 0 );
    /* The goto below is used to avoid an "unused label" warning.*/
    goto exit;
exit:
    ;
}

void test_app_data_tls_wrapper( void ** params )
{

    test_app_data_tls( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ) );
}
#endif /* MBEDTLS_CTR_DRBG_C */
#endif /* MBEDTLS_ENTROPY_C */
#endif /* MBEDTLS_ECP_DP_SECP384R1_ENABLED */
#endif /* MBEDTLS_RSA_C */
#endif /* MBEDTLS_SSL_PROTO_TLS1_2 */
#endif /* MBEDTLS_PKCS1_V15 */
#endif /* !MBEDTLS_USE_PSA_CRYPTO */
#endif /* MBEDTLS_X509_CRT_PARSE_C */
#if defined(MBEDTLS_X509_CRT_PARSE_C)
#if !defined(MBEDTLS_USE_PSA_CRYPTO)
#if defined(MBEDTLS_PKCS1_V15)
#if defined(MBEDTLS_SSL_PROTO_TLS1_2)
#if defined(MBEDTLS_RSA_C)
#if defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED)
#if defined(MBEDTLS_SSL_PROTO_DTLS)
#if defined(MBEDTLS_ENTROPY_C)
#if defined(MBEDTLS_CTR_DRBG_C)
void test_app_data_dtls( int mfl, int cli_msg_len, int srv_msg_len,
                    int expected_cli_fragments,
                    int expected_srv_fragments )
{
    test_app_data( mfl, cli_msg_len, srv_msg_len, expected_cli_fragments,
                   expected_srv_fragments, 1 );
    /* The goto below is used to avoid an "unused label" warning.*/
    goto exit;
exit:
    ;
}

void test_app_data_dtls_wrapper( void ** params )
{

    test_app_data_dtls( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ) );
}
#endif /* MBEDTLS_CTR_DRBG_C */
#endif /* MBEDTLS_ENTROPY_C */
#endif /* MBEDTLS_SSL_PROTO_DTLS */
#endif /* MBEDTLS_ECP_DP_SECP384R1_ENABLED */
#endif /* MBEDTLS_RSA_C */
#endif /* MBEDTLS_SSL_PROTO_TLS1_2 */
#endif /* MBEDTLS_PKCS1_V15 */
#endif /* !MBEDTLS_USE_PSA_CRYPTO */
#endif /* MBEDTLS_X509_CRT_PARSE_C */
#if defined(MBEDTLS_X509_CRT_PARSE_C)
#if !defined(MBEDTLS_USE_PSA_CRYPTO)
#if defined(MBEDTLS_PKCS1_V15)
#if defined(MBEDTLS_SSL_PROTO_TLS1_2)
#if defined(MBEDTLS_RSA_C)
#if defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED)
#if defined(MBEDTLS_SSL_PROTO_DTLS)
#if defined(MBEDTLS_SSL_RENEGOTIATION)
#if defined(MBEDTLS_SSL_CONTEXT_SERIALIZATION)
#if defined(MBEDTLS_ENTROPY_C)
#if defined(MBEDTLS_CTR_DRBG_C)
void test_handshake_serialization( )
{
    handshake_test_options options;
    init_handshake_options( &options );

    options.serialize = 1;
    options.dtls = 1;
    perform_handshake( &options );
    /* The goto below is used to avoid an "unused label" warning.*/
    goto exit;
exit:
    ;
}

void test_handshake_serialization_wrapper( void ** params )
{
    (void)params;

    test_handshake_serialization(  );
}
#endif /* MBEDTLS_CTR_DRBG_C */
#endif /* MBEDTLS_ENTROPY_C */
#endif /* MBEDTLS_SSL_CONTEXT_SERIALIZATION */
#endif /* MBEDTLS_SSL_RENEGOTIATION */
#endif /* MBEDTLS_SSL_PROTO_DTLS */
#endif /* MBEDTLS_ECP_DP_SECP384R1_ENABLED */
#endif /* MBEDTLS_RSA_C */
#endif /* MBEDTLS_SSL_PROTO_TLS1_2 */
#endif /* MBEDTLS_PKCS1_V15 */
#endif /* !MBEDTLS_USE_PSA_CRYPTO */
#endif /* MBEDTLS_X509_CRT_PARSE_C */
#if defined(MBEDTLS_X509_CRT_PARSE_C)
#if !defined(MBEDTLS_USE_PSA_CRYPTO)
#if defined(MBEDTLS_PKCS1_V15)
#if defined(MBEDTLS_RSA_C)
#if defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED)
#if defined(MBEDTLS_DEBUG_C)
#if defined(MBEDTLS_SSL_MAX_FRAGMENT_LENGTH)
#if defined(MBEDTLS_CIPHER_MODE_CBC)
#if defined(MBEDTLS_ENTROPY_C)
#if defined(MBEDTLS_CTR_DRBG_C)
void test_handshake_fragmentation( int mfl, int expected_srv_hs_fragmentation, int expected_cli_hs_fragmentation)
{
    handshake_test_options options;
    log_pattern srv_pattern, cli_pattern;

    srv_pattern.pattern = cli_pattern.pattern = "found fragmented DTLS handshake";
    srv_pattern.counter = 0;
    cli_pattern.counter = 0;

    init_handshake_options( &options );
    options.dtls = 1;
    options.mfl = mfl;
    /* Set cipher to one using CBC so that record splitting can be tested */
    options.cipher = "DHE-RSA-AES256-CBC-SHA256";
    options.srv_auth_mode = MBEDTLS_SSL_VERIFY_REQUIRED;
    options.srv_log_obj = &srv_pattern;
    options.cli_log_obj = &cli_pattern;
    options.srv_log_fun = log_analyzer;
    options.cli_log_fun = log_analyzer;

    perform_handshake( &options );

    /* Test if the server received a fragmented handshake */
    if( expected_srv_hs_fragmentation )
    {
        TEST_ASSERT( srv_pattern.counter >= 1 );
    }
    /* Test if the client received a fragmented handshake */
    if( expected_cli_hs_fragmentation )
    {
        TEST_ASSERT( cli_pattern.counter >= 1 );
    }
exit:
    ;
}

void test_handshake_fragmentation_wrapper( void ** params )
{

    test_handshake_fragmentation( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ) );
}
#endif /* MBEDTLS_CTR_DRBG_C */
#endif /* MBEDTLS_ENTROPY_C */
#endif /* MBEDTLS_CIPHER_MODE_CBC */
#endif /* MBEDTLS_SSL_MAX_FRAGMENT_LENGTH */
#endif /* MBEDTLS_DEBUG_C */
#endif /* MBEDTLS_ECP_DP_SECP384R1_ENABLED */
#endif /* MBEDTLS_RSA_C */
#endif /* MBEDTLS_PKCS1_V15 */
#endif /* !MBEDTLS_USE_PSA_CRYPTO */
#endif /* MBEDTLS_X509_CRT_PARSE_C */
#if defined(MBEDTLS_X509_CRT_PARSE_C)
#if !defined(MBEDTLS_USE_PSA_CRYPTO)
#if defined(MBEDTLS_PKCS1_V15)
#if defined(MBEDTLS_SSL_PROTO_TLS1_2)
#if defined(MBEDTLS_RSA_C)
#if defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED)
#if defined(MBEDTLS_SSL_PROTO_DTLS)
#if defined(MBEDTLS_SSL_RENEGOTIATION)
#if defined(MBEDTLS_ENTROPY_C)
#if defined(MBEDTLS_CTR_DRBG_C)
void test_renegotiation( int legacy_renegotiation )
{
    handshake_test_options options;
    init_handshake_options( &options );

    options.renegotiate = 1;
    options.legacy_renegotiation = legacy_renegotiation;
    options.dtls = 1;

    perform_handshake( &options );
    /* The goto below is used to avoid an "unused label" warning.*/
    goto exit;
exit:
    ;
}

void test_renegotiation_wrapper( void ** params )
{

    test_renegotiation( *( (int *) params[0] ) );
}
#endif /* MBEDTLS_CTR_DRBG_C */
#endif /* MBEDTLS_ENTROPY_C */
#endif /* MBEDTLS_SSL_RENEGOTIATION */
#endif /* MBEDTLS_SSL_PROTO_DTLS */
#endif /* MBEDTLS_ECP_DP_SECP384R1_ENABLED */
#endif /* MBEDTLS_RSA_C */
#endif /* MBEDTLS_SSL_PROTO_TLS1_2 */
#endif /* MBEDTLS_PKCS1_V15 */
#endif /* !MBEDTLS_USE_PSA_CRYPTO */
#endif /* MBEDTLS_X509_CRT_PARSE_C */
#if defined(MBEDTLS_X509_CRT_PARSE_C)
#if !defined(MBEDTLS_USE_PSA_CRYPTO)
#if defined(MBEDTLS_PKCS1_V15)
#if defined(MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH)
#if defined(MBEDTLS_SSL_PROTO_TLS1_2)
#if defined(MBEDTLS_RSA_C)
#if defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED)
#if defined(MBEDTLS_ENTROPY_C)
#if defined(MBEDTLS_CTR_DRBG_C)
void test_resize_buffers( int mfl, int renegotiation, int legacy_renegotiation,
                     int serialize, int dtls, char *cipher )
{
    handshake_test_options options;
    init_handshake_options( &options );

    options.mfl = mfl;
    options.cipher = cipher;
    options.renegotiate = renegotiation;
    options.legacy_renegotiation = legacy_renegotiation;
    options.serialize = serialize;
    options.dtls = dtls;
    options.resize_buffers = 1;

    perform_handshake( &options );
    /* The goto below is used to avoid an "unused label" warning.*/
    goto exit;
exit:
    ;
}

void test_resize_buffers_wrapper( void ** params )
{

    test_resize_buffers( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ), (char *) params[5] );
}
#endif /* MBEDTLS_CTR_DRBG_C */
#endif /* MBEDTLS_ENTROPY_C */
#endif /* MBEDTLS_ECP_DP_SECP384R1_ENABLED */
#endif /* MBEDTLS_RSA_C */
#endif /* MBEDTLS_SSL_PROTO_TLS1_2 */
#endif /* MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH */
#endif /* MBEDTLS_PKCS1_V15 */
#endif /* !MBEDTLS_USE_PSA_CRYPTO */
#endif /* MBEDTLS_X509_CRT_PARSE_C */
#if defined(MBEDTLS_X509_CRT_PARSE_C)
#if !defined(MBEDTLS_USE_PSA_CRYPTO)
#if defined(MBEDTLS_PKCS1_V15)
#if defined(MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH)
#if defined(MBEDTLS_SSL_CONTEXT_SERIALIZATION)
#if defined(MBEDTLS_SSL_PROTO_TLS1_2)
#if defined(MBEDTLS_RSA_C)
#if defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED)
#if defined(MBEDTLS_SSL_PROTO_DTLS)
#if defined(MBEDTLS_ENTROPY_C)
#if defined(MBEDTLS_CTR_DRBG_C)
void test_resize_buffers_serialize_mfl( int mfl )
{
    test_resize_buffers( mfl, 0, MBEDTLS_SSL_LEGACY_NO_RENEGOTIATION, 1, 1,
                         (char *) "" );

    /* The goto below is used to avoid an "unused label" warning.*/
    goto exit;
exit:
    ;
}

void test_resize_buffers_serialize_mfl_wrapper( void ** params )
{

    test_resize_buffers_serialize_mfl( *( (int *) params[0] ) );
}
#endif /* MBEDTLS_CTR_DRBG_C */
#endif /* MBEDTLS_ENTROPY_C */
#endif /* MBEDTLS_SSL_PROTO_DTLS */
#endif /* MBEDTLS_ECP_DP_SECP384R1_ENABLED */
#endif /* MBEDTLS_RSA_C */
#endif /* MBEDTLS_SSL_PROTO_TLS1_2 */
#endif /* MBEDTLS_SSL_CONTEXT_SERIALIZATION */
#endif /* MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH */
#endif /* MBEDTLS_PKCS1_V15 */
#endif /* !MBEDTLS_USE_PSA_CRYPTO */
#endif /* MBEDTLS_X509_CRT_PARSE_C */
#if defined(MBEDTLS_X509_CRT_PARSE_C)
#if !defined(MBEDTLS_USE_PSA_CRYPTO)
#if defined(MBEDTLS_PKCS1_V15)
#if defined(MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH)
#if defined(MBEDTLS_SSL_RENEGOTIATION)
#if defined(MBEDTLS_SSL_PROTO_TLS1_2)
#if defined(MBEDTLS_RSA_C)
#if defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED)
#if defined(MBEDTLS_ENTROPY_C)
#if defined(MBEDTLS_CTR_DRBG_C)
void test_resize_buffers_renegotiate_mfl( int mfl, int legacy_renegotiation,
                                     char *cipher )
{
    test_resize_buffers( mfl, 1, legacy_renegotiation, 0, 1, cipher );

    /* The goto below is used to avoid an "unused label" warning.*/
    goto exit;
exit:
    ;
}

void test_resize_buffers_renegotiate_mfl_wrapper( void ** params )
{

    test_resize_buffers_renegotiate_mfl( *( (int *) params[0] ), *( (int *) params[1] ), (char *) params[2] );
}
#endif /* MBEDTLS_CTR_DRBG_C */
#endif /* MBEDTLS_ENTROPY_C */
#endif /* MBEDTLS_ECP_DP_SECP384R1_ENABLED */
#endif /* MBEDTLS_RSA_C */
#endif /* MBEDTLS_SSL_PROTO_TLS1_2 */
#endif /* MBEDTLS_SSL_RENEGOTIATION */
#endif /* MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH */
#endif /* MBEDTLS_PKCS1_V15 */
#endif /* !MBEDTLS_USE_PSA_CRYPTO */
#endif /* MBEDTLS_X509_CRT_PARSE_C */
#if defined(MBEDTLS_SSL_SOME_SUITES_USE_TLS_CBC)
#if defined(MBEDTLS_TEST_HOOKS)
void test_ssl_cf_hmac( int hash )
{
    /*
     * Test the function mbedtls_ssl_cf_hmac() against a reference
     * implementation.
     */
    mbedtls_md_context_t ctx, ref_ctx;
    const mbedtls_md_info_t *md_info;
    size_t out_len, block_size;
    size_t min_in_len, in_len, max_in_len, i;
    /* TLS additional data is 13 bytes (hence the "lucky 13" name) */
    unsigned char add_data[13];
    unsigned char ref_out[MBEDTLS_MD_MAX_SIZE];
    unsigned char *data = NULL;
    unsigned char *out = NULL;
    unsigned char rec_num = 0;

    mbedtls_md_init( &ctx );
    mbedtls_md_init( &ref_ctx );

    md_info = mbedtls_md_info_from_type( hash );
    TEST_ASSERT( md_info != NULL );
    out_len = mbedtls_md_get_size( md_info );
    TEST_ASSERT( out_len != 0 );
    block_size = hash == MBEDTLS_MD_SHA384 ? 128 : 64;

    /* Use allocated out buffer to catch overwrites */
    ASSERT_ALLOC( out, out_len );

    /* Set up contexts with the given hash and a dummy key */
    TEST_EQUAL( 0, mbedtls_md_setup( &ctx, md_info, 1 ) );
    TEST_EQUAL( 0, mbedtls_md_setup( &ref_ctx, md_info, 1 ) );
    memset( ref_out, 42, sizeof( ref_out ) );
    TEST_EQUAL( 0, mbedtls_md_hmac_starts( &ctx, ref_out, out_len ) );
    TEST_EQUAL( 0, mbedtls_md_hmac_starts( &ref_ctx, ref_out, out_len ) );
    memset( ref_out, 0, sizeof( ref_out ) );

    /*
     * Test all possible lengths up to a point. The difference between
     * max_in_len and min_in_len is at most 255, and make sure they both vary
     * by at least one block size.
     */
    for( max_in_len = 0; max_in_len <= 255 + block_size; max_in_len++ )
    {
        mbedtls_test_set_step( max_in_len * 10000 );

        /* Use allocated in buffer to catch overreads */
        ASSERT_ALLOC( data, max_in_len );

        min_in_len = max_in_len > 255 ? max_in_len - 255 : 0;
        for( in_len = min_in_len; in_len <= max_in_len; in_len++ )
        {
            mbedtls_test_set_step( max_in_len * 10000 + in_len );

            /* Set up dummy data and add_data */
            rec_num++;
            memset( add_data, rec_num, sizeof( add_data ) );
            for( i = 0; i < in_len; i++ )
                data[i] = ( i & 0xff ) ^ rec_num;

            /* Get the function's result */
            TEST_CF_SECRET( &in_len, sizeof( in_len ) );
            TEST_EQUAL( 0, mbedtls_ssl_cf_hmac( &ctx, add_data, sizeof( add_data ),
                                                data, in_len,
                                                min_in_len, max_in_len,
                                                out ) );
            TEST_CF_PUBLIC( &in_len, sizeof( in_len ) );
            TEST_CF_PUBLIC( out, out_len );

            /* Compute the reference result */
            TEST_EQUAL( 0, mbedtls_md_hmac_update( &ref_ctx, add_data,
                                                   sizeof( add_data ) ) );
            TEST_EQUAL( 0, mbedtls_md_hmac_update( &ref_ctx, data, in_len ) );
            TEST_EQUAL( 0, mbedtls_md_hmac_finish( &ref_ctx, ref_out ) );
            TEST_EQUAL( 0, mbedtls_md_hmac_reset( &ref_ctx ) );

            /* Compare */
            ASSERT_COMPARE( out, out_len, ref_out, out_len );
        }

        mbedtls_free( data );
        data = NULL;
    }

exit:
    mbedtls_md_free( &ref_ctx );
    mbedtls_md_free( &ctx );

    mbedtls_free( data );
    mbedtls_free( out );
}

void test_ssl_cf_hmac_wrapper( void ** params )
{

    test_ssl_cf_hmac( *( (int *) params[0] ) );
}
#endif /* MBEDTLS_TEST_HOOKS */
#endif /* MBEDTLS_SSL_SOME_SUITES_USE_TLS_CBC */
#if defined(MBEDTLS_SSL_SOME_SUITES_USE_TLS_CBC)
#if defined(MBEDTLS_TEST_HOOKS)
void test_ssl_cf_memcpy_offset( int offset_min, int offset_max, int len )
{
    unsigned char *dst = NULL;
    unsigned char *src = NULL;
    size_t src_len = offset_max + len;
    size_t secret;

    ASSERT_ALLOC( dst, len );
    ASSERT_ALLOC( src, src_len );

    /* Fill src in a way that we can detect if we copied the right bytes */
    mbedtls_test_rnd_std_rand( NULL, src, src_len );

    for( secret = offset_min; secret <= (size_t) offset_max; secret++ )
    {
        mbedtls_test_set_step( (int) secret );

        TEST_CF_SECRET( &secret, sizeof( secret ) );
        mbedtls_ssl_cf_memcpy_offset( dst, src, secret,
                                      offset_min, offset_max, len );
        TEST_CF_PUBLIC( &secret, sizeof( secret ) );
        TEST_CF_PUBLIC( dst, len );

        ASSERT_COMPARE( dst, len, src + secret, len );
    }

exit:
    mbedtls_free( dst );
    mbedtls_free( src );
}

void test_ssl_cf_memcpy_offset_wrapper( void ** params )
{

    test_ssl_cf_memcpy_offset( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ) );
}
#endif /* MBEDTLS_TEST_HOOKS */
#endif /* MBEDTLS_SSL_SOME_SUITES_USE_TLS_CBC */
#endif /* MBEDTLS_SSL_TLS_C */

/*----------------------------------------------------------------------------*/
/* Test dispatch code */


/**
 * \brief       Evaluates an expression/macro into its literal integer value.
 *              For optimizing space for embedded targets each expression/macro
 *              is identified by a unique identifier instead of string literals.
 *              Identifiers and evaluation code is generated by script:
 *              generate_test_code.py and then mbedtls_test_suite.sh and then mbedtls_test_suite.sh
 *
 * \param exp_id    Expression identifier.
 * \param out_value Pointer to int to hold the integer.
 *
 * \return       0 if exp_id is found. 1 otherwise.
 */
int get_expression( int32_t exp_id, int32_t * out_value )
{
    int ret = KEY_VALUE_MAPPING_FOUND;

    (void) exp_id;
    (void) out_value;

    switch( exp_id )
    {

#if defined(MBEDTLS_SSL_TLS_C)

        case 0:
            {
                *out_value = MBEDTLS_SSL_IS_CLIENT;
            }
            break;
        case 1:
            {
                *out_value = MBEDTLS_SSL_IS_SERVER;
            }
            break;
        case 2:
            {
                *out_value = MBEDTLS_SSL_HELLO_REQUEST;
            }
            break;
        case 3:
            {
                *out_value = MBEDTLS_SSL_CLIENT_HELLO;
            }
            break;
        case 4:
            {
                *out_value = MBEDTLS_SSL_SERVER_HELLO;
            }
            break;
        case 5:
            {
                *out_value = MBEDTLS_SSL_SERVER_CERTIFICATE;
            }
            break;
        case 6:
            {
                *out_value = MBEDTLS_SSL_SERVER_KEY_EXCHANGE;
            }
            break;
        case 7:
            {
                *out_value = MBEDTLS_SSL_CERTIFICATE_REQUEST;
            }
            break;
        case 8:
            {
                *out_value = MBEDTLS_SSL_SERVER_HELLO_DONE;
            }
            break;
        case 9:
            {
                *out_value = MBEDTLS_SSL_CLIENT_CERTIFICATE;
            }
            break;
        case 10:
            {
                *out_value = MBEDTLS_SSL_CLIENT_KEY_EXCHANGE;
            }
            break;
        case 11:
            {
                *out_value = MBEDTLS_SSL_CERTIFICATE_VERIFY;
            }
            break;
        case 12:
            {
                *out_value = MBEDTLS_SSL_CLIENT_CHANGE_CIPHER_SPEC;
            }
            break;
        case 13:
            {
                *out_value = MBEDTLS_SSL_CLIENT_FINISHED;
            }
            break;
        case 14:
            {
                *out_value = MBEDTLS_SSL_SERVER_CHANGE_CIPHER_SPEC;
            }
            break;
        case 15:
            {
                *out_value = MBEDTLS_SSL_SERVER_FINISHED;
            }
            break;
        case 16:
            {
                *out_value = MBEDTLS_SSL_FLUSH_BUFFERS;
            }
            break;
        case 17:
            {
                *out_value = MBEDTLS_SSL_HANDSHAKE_WRAPUP;
            }
            break;
        case 18:
            {
                *out_value = MBEDTLS_SSL_HANDSHAKE_OVER;
            }
            break;
        case 19:
            {
                *out_value = MBEDTLS_SSL_SERVER_HELLO_VERIFY_REQUEST_SENT;
            }
            break;
        case 20:
            {
                *out_value = MBEDTLS_SSL_SERVER_NEW_SESSION_TICKET;
            }
            break;
        case 21:
            {
                *out_value = MBEDTLS_SSL_MINOR_VERSION_0;
            }
            break;
        case 22:
            {
                *out_value = MBEDTLS_SSL_MINOR_VERSION_1;
            }
            break;
        case 23:
            {
                *out_value = MBEDTLS_SSL_MINOR_VERSION_2;
            }
            break;
        case 24:
            {
                *out_value = MBEDTLS_SSL_MINOR_VERSION_3;
            }
            break;
        case 25:
            {
                *out_value = MBEDTLS_PK_RSA;
            }
            break;
        case 26:
            {
                *out_value = MBEDTLS_PK_ECDSA;
            }
            break;
        case 27:
            {
                *out_value = MBEDTLS_SSL_MAX_FRAG_LEN_512;
            }
            break;
        case 28:
            {
                *out_value = MBEDTLS_SSL_MAX_FRAG_LEN_1024;
            }
            break;
        case 29:
            {
                *out_value = TEST_SSL_MINOR_VERSION_NONE;
            }
            break;
        case 30:
            {
                *out_value = MBEDTLS_SSL_MAX_FRAG_LEN_2048;
            }
            break;
        case 31:
            {
                *out_value = MBEDTLS_SSL_MAX_FRAG_LEN_4096;
            }
            break;
        case 32:
            {
                *out_value = MBEDTLS_SSL_MAX_FRAG_LEN_NONE;
            }
            break;
        case 33:
            {
                *out_value = MBEDTLS_SSL_LEGACY_NO_RENEGOTIATION;
            }
            break;
        case 34:
            {
                *out_value = MBEDTLS_SSL_LEGACY_ALLOW_RENEGOTIATION;
            }
            break;
        case 35:
            {
                *out_value = MBEDTLS_SSL_LEGACY_BREAK_HANDSHAKE;
            }
            break;
        case 36:
            {
                *out_value = -1;
            }
            break;
        case 37:
            {
                *out_value = MBEDTLS_CIPHER_AES_128_CBC;
            }
            break;
        case 38:
            {
                *out_value = MBEDTLS_MD_SHA384;
            }
            break;
        case 39:
            {
                *out_value = MBEDTLS_MD_SHA256;
            }
            break;
        case 40:
            {
                *out_value = MBEDTLS_MD_SHA1;
            }
            break;
        case 41:
            {
                *out_value = MBEDTLS_MD_MD5;
            }
            break;
        case 42:
            {
                *out_value = MBEDTLS_CIPHER_AES_192_CBC;
            }
            break;
        case 43:
            {
                *out_value = MBEDTLS_CIPHER_AES_256_CBC;
            }
            break;
        case 44:
            {
                *out_value = MBEDTLS_CIPHER_ARIA_128_CBC;
            }
            break;
        case 45:
            {
                *out_value = MBEDTLS_CIPHER_ARIA_192_CBC;
            }
            break;
        case 46:
            {
                *out_value = MBEDTLS_CIPHER_ARIA_256_CBC;
            }
            break;
        case 47:
            {
                *out_value = MBEDTLS_CIPHER_CAMELLIA_128_CBC;
            }
            break;
        case 48:
            {
                *out_value = MBEDTLS_CIPHER_CAMELLIA_192_CBC;
            }
            break;
        case 49:
            {
                *out_value = MBEDTLS_CIPHER_CAMELLIA_256_CBC;
            }
            break;
        case 50:
            {
                *out_value = MBEDTLS_CIPHER_BLOWFISH_CBC;
            }
            break;
        case 51:
            {
                *out_value = MBEDTLS_CIPHER_AES_128_GCM;
            }
            break;
        case 52:
            {
                *out_value = MBEDTLS_SSL_MINOR_VERSION_4;
            }
            break;
        case 53:
            {
                *out_value = MBEDTLS_CIPHER_AES_192_GCM;
            }
            break;
        case 54:
            {
                *out_value = MBEDTLS_CIPHER_AES_256_GCM;
            }
            break;
        case 55:
            {
                *out_value = MBEDTLS_CIPHER_CAMELLIA_128_GCM;
            }
            break;
        case 56:
            {
                *out_value = MBEDTLS_CIPHER_CAMELLIA_192_GCM;
            }
            break;
        case 57:
            {
                *out_value = MBEDTLS_CIPHER_CAMELLIA_256_GCM;
            }
            break;
        case 58:
            {
                *out_value = MBEDTLS_CIPHER_AES_128_CCM;
            }
            break;
        case 59:
            {
                *out_value = MBEDTLS_CIPHER_AES_192_CCM;
            }
            break;
        case 60:
            {
                *out_value = MBEDTLS_CIPHER_AES_256_CCM;
            }
            break;
        case 61:
            {
                *out_value = MBEDTLS_CIPHER_CAMELLIA_128_CCM;
            }
            break;
        case 62:
            {
                *out_value = MBEDTLS_CIPHER_CAMELLIA_192_CCM;
            }
            break;
        case 63:
            {
                *out_value = MBEDTLS_CIPHER_CAMELLIA_256_CCM;
            }
            break;
        case 64:
            {
                *out_value = MBEDTLS_CIPHER_ARC4_128;
            }
            break;
        case 65:
            {
                *out_value = MBEDTLS_CIPHER_NULL;
            }
            break;
        case 66:
            {
                *out_value = MBEDTLS_CIPHER_CHACHA20_POLY1305;
            }
            break;
        case 67:
            {
                *out_value = -2;
            }
            break;
        case 68:
            {
                *out_value = MBEDTLS_CIPHER_DES_EDE3_CBC;
            }
            break;
        case 69:
            {
                *out_value = tls1_3_label_key;
            }
            break;
        case 70:
            {
                *out_value = tls1_3_label_iv;
            }
            break;
        case 71:
            {
                *out_value = tls1_3_label_finished;
            }
            break;
        case 72:
            {
                *out_value = tls1_3_label_resumption;
            }
            break;
        case 73:
            {
                *out_value = tls1_3_label_derived;
            }
            break;
        case 74:
            {
                *out_value = MBEDTLS_SSL_TLS1_3_CONTEXT_UNHASHED;
            }
            break;
        case 75:
            {
                *out_value = tls1_3_label_s_ap_traffic;
            }
            break;
        case 76:
            {
                *out_value = MBEDTLS_SSL_TLS1_3_CONTEXT_HASHED;
            }
            break;
        case 77:
            {
                *out_value = tls1_3_label_c_e_traffic;
            }
            break;
        case 78:
            {
                *out_value = tls1_3_label_e_exp_master;
            }
            break;
        case 79:
            {
                *out_value = tls1_3_label_c_hs_traffic;
            }
            break;
        case 80:
            {
                *out_value = tls1_3_label_s_hs_traffic;
            }
            break;
        case 81:
            {
                *out_value = tls1_3_label_c_ap_traffic;
            }
            break;
        case 82:
            {
                *out_value = tls1_3_label_exp_master;
            }
            break;
        case 83:
            {
                *out_value = tls1_3_label_res_master;
            }
            break;
        case 84:
            {
                *out_value = MBEDTLS_SSL_TLS_PRF_NONE;
            }
            break;
        case 85:
            {
                *out_value = MBEDTLS_ERR_SSL_FEATURE_UNAVAILABLE;
            }
            break;
        case 86:
            {
                *out_value = MBEDTLS_SSL_TLS_PRF_SSL3;
            }
            break;
        case 87:
            {
                *out_value = MBEDTLS_SSL_TLS_PRF_TLS1;
            }
            break;
        case 88:
            {
                *out_value = MBEDTLS_SSL_TLS_PRF_SHA384;
            }
            break;
        case 89:
            {
                *out_value = MBEDTLS_SSL_TLS_PRF_SHA256;
            }
            break;
#endif

        default:
           {
                ret = KEY_VALUE_MAPPING_NOT_FOUND;
           }
           break;
    }
    return( ret );
}


/**
 * \brief       Checks if the dependency i.e. the compile flag is set.
 *              For optimizing space for embedded targets each dependency
 *              is identified by a unique identifier instead of string literals.
 *              Identifiers and check code is generated by script:
 *              generate_test_code.py and then mbedtls_test_suite.sh and then mbedtls_test_suite.sh
 *
 * \param dep_id    Dependency identifier.
 *
 * \return       DEPENDENCY_SUPPORTED if set else DEPENDENCY_NOT_SUPPORTED
 */
int dep_check( int dep_id )
{
    int ret = DEPENDENCY_NOT_SUPPORTED;

    (void) dep_id;

    switch( dep_id )
    {

#if defined(MBEDTLS_SSL_TLS_C)

        case 0:
            {
#if defined(MBEDTLS_SSL_PROTO_SSL3)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 1:
            {
#if defined(MBEDTLS_RSA_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 2:
            {
#if defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 3:
            {
#if defined(MBEDTLS_SSL_PROTO_TLS1)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 4:
            {
#if defined(MBEDTLS_CIPHER_MODE_CBC)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 5:
            {
#if defined(MBEDTLS_SSL_PROTO_TLS1_1)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 6:
            {
#if defined(MBEDTLS_SSL_PROTO_TLS1_2)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 7:
            {
#if defined(MBEDTLS_SHA512_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 8:
            {
#if !defined(MBEDTLS_SHA512_NO_SHA384)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 9:
            {
#if defined(MBEDTLS_AES_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 10:
            {
#if defined(MBEDTLS_GCM_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 11:
            {
#if defined(MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 12:
            {
#if defined(MBEDTLS_CCM_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 13:
            {
#if defined(MBEDTLS_SHA256_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 14:
            {
#if defined(MBEDTLS_ECDSA_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 15:
            {
#if defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 16:
            {
#if defined(MBEDTLS_CAMELLIA_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 17:
            {
#if defined(MBEDTLS_SSL_PROTO_DTLS)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 18:
            {
#if defined(MBEDTLS_SSL_MAX_FRAGMENT_LENGTH)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 19:
            {
#if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 20:
            {
#if defined(MBEDTLS_SSL_ENCRYPT_THEN_MAC)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 21:
            {
#if defined(MBEDTLS_SHA1_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 22:
            {
#if defined(MBEDTLS_MD5_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 23:
            {
#if defined(MBEDTLS_ARIA_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 24:
            {
#if defined(MBEDTLS_BLOWFISH_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 25:
            {
#if defined(MBEDTLS_SSL_PROTO_TLS1_3_EXPERIMENTAL)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 26:
            {
#if defined(MBEDTLS_ARC4_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 27:
            {
#if defined(MBEDTLS_CIPHER_NULL_CIPHER)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 28:
            {
#if defined(MBEDTLS_CHACHAPOLY_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 29:
            {
#if defined(MBEDTLS_DES_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 30:
            {
#if !defined(MBEDTLS_SSL_PROTO_SSL3)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 31:
            {
#if !defined(MBEDTLS_SSL_PROTO_TLS1)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 32:
            {
#if !defined(MBEDTLS_SSL_PROTO_TLS1_1)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 33:
            {
#if !defined(MBEDTLS_SHA512_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 34:
            {
#if !defined(MBEDTLS_SHA256_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 35:
            {
#if defined(MBEDTLS_SSL_SESSION_TICKETS)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 36:
            {
#if defined(MBEDTLS_SSL_CLI_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 37:
            {
#if defined(MBEDTLS_X509_USE_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 38:
            {
#if defined(MBEDTLS_PEM_PARSE_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;

        case 39:
            {
#if defined(MBEDTLS_FS_IO)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;

        case 40:
            {
#if defined(MBEDTLS_KEY_EXCHANGE_DHE_RSA_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;

        case 41:
            {
#if defined(MBEDTLS_KEY_EXCHANGE_PSK_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;

#endif
        default:
            break;
    }
    return( ret );
}


/**
 * \brief       Function pointer type for test function wrappers.
 *
 * A test function wrapper decodes the parameters and passes them to the
 * underlying test function. Both the wrapper and the underlying function
 * return void. Test wrappers assume that they are passed a suitable
 * parameter array and do not perform any error detection.
 *
 * \param param_array   The array of parameters. Each element is a `void *`
 *                      which the wrapper casts to the correct type and
 *                      dereferences. Each wrapper function hard-codes the
 *                      number and types of the parameters.
 */
typedef void (*TestWrapper_t)( void **param_array );


/**
 * \brief       Table of test function wrappers. Used by dispatch_test().
 *              This table is populated by script:
 *              generate_test_code.py and then mbedtls_test_suite.sh and then mbedtls_test_suite.sh
 *
 */
TestWrapper_t test_funcs[] =
{
/* Function Id: 0 */

#if defined(MBEDTLS_SSL_TLS_C)
    test_test_callback_buffer_sanity_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_SSL_TLS_C)
    test_test_callback_buffer_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_SSL_TLS_C)
    test_ssl_mock_sanity_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_SSL_TLS_C)
    test_ssl_mock_tcp_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_SSL_TLS_C)
    test_ssl_mock_tcp_interleaving_wrapper,
#else
    NULL,
#endif
/* Function Id: 5 */

#if defined(MBEDTLS_SSL_TLS_C)
    test_ssl_message_queue_sanity_wrapper,
#else
    NULL,
#endif
/* Function Id: 6 */

#if defined(MBEDTLS_SSL_TLS_C)
    test_ssl_message_queue_basic_wrapper,
#else
    NULL,
#endif
/* Function Id: 7 */

#if defined(MBEDTLS_SSL_TLS_C)
    test_ssl_message_queue_overflow_underflow_wrapper,
#else
    NULL,
#endif
/* Function Id: 8 */

#if defined(MBEDTLS_SSL_TLS_C)
    test_ssl_message_queue_interleaved_wrapper,
#else
    NULL,
#endif
/* Function Id: 9 */

#if defined(MBEDTLS_SSL_TLS_C)
    test_ssl_message_queue_insufficient_buffer_wrapper,
#else
    NULL,
#endif
/* Function Id: 10 */

#if defined(MBEDTLS_SSL_TLS_C)
    test_ssl_message_mock_uninitialized_wrapper,
#else
    NULL,
#endif
/* Function Id: 11 */

#if defined(MBEDTLS_SSL_TLS_C)
    test_ssl_message_mock_basic_wrapper,
#else
    NULL,
#endif
/* Function Id: 12 */

#if defined(MBEDTLS_SSL_TLS_C)
    test_ssl_message_mock_queue_overflow_underflow_wrapper,
#else
    NULL,
#endif
/* Function Id: 13 */

#if defined(MBEDTLS_SSL_TLS_C)
    test_ssl_message_mock_socket_overflow_wrapper,
#else
    NULL,
#endif
/* Function Id: 14 */

#if defined(MBEDTLS_SSL_TLS_C)
    test_ssl_message_mock_truncated_wrapper,
#else
    NULL,
#endif
/* Function Id: 15 */

#if defined(MBEDTLS_SSL_TLS_C)
    test_ssl_message_mock_socket_read_error_wrapper,
#else
    NULL,
#endif
/* Function Id: 16 */

#if defined(MBEDTLS_SSL_TLS_C)
    test_ssl_message_mock_interleaved_one_way_wrapper,
#else
    NULL,
#endif
/* Function Id: 17 */

#if defined(MBEDTLS_SSL_TLS_C)
    test_ssl_message_mock_interleaved_two_ways_wrapper,
#else
    NULL,
#endif
/* Function Id: 18 */

#if defined(MBEDTLS_SSL_TLS_C) && defined(MBEDTLS_SSL_DTLS_ANTI_REPLAY)
    test_ssl_dtls_replay_wrapper,
#else
    NULL,
#endif
/* Function Id: 19 */

#if defined(MBEDTLS_SSL_TLS_C) && defined(MBEDTLS_X509_CRT_PARSE_C)
    test_ssl_set_hostname_twice_wrapper,
#else
    NULL,
#endif
/* Function Id: 20 */

#if defined(MBEDTLS_SSL_TLS_C)
    test_ssl_crypt_record_wrapper,
#else
    NULL,
#endif
/* Function Id: 21 */

#if defined(MBEDTLS_SSL_TLS_C)
    test_ssl_crypt_record_small_wrapper,
#else
    NULL,
#endif
/* Function Id: 22 */

#if defined(MBEDTLS_SSL_TLS_C) && defined(MBEDTLS_CIPHER_MODE_CBC) && defined(MBEDTLS_AES_C) && defined(MBEDTLS_SSL_PROTO_TLS1_2)
    test_ssl_decrypt_non_etm_cbc_wrapper,
#else
    NULL,
#endif
/* Function Id: 23 */

#if defined(MBEDTLS_SSL_TLS_C) && defined(MBEDTLS_SSL_PROTO_TLS1_3_EXPERIMENTAL)
    test_ssl_tls1_3_hkdf_expand_label_wrapper,
#else
    NULL,
#endif
/* Function Id: 24 */

#if defined(MBEDTLS_SSL_TLS_C) && defined(MBEDTLS_SSL_PROTO_TLS1_3_EXPERIMENTAL)
    test_ssl_tls1_3_traffic_key_generation_wrapper,
#else
    NULL,
#endif
/* Function Id: 25 */

#if defined(MBEDTLS_SSL_TLS_C) && defined(MBEDTLS_SSL_PROTO_TLS1_3_EXPERIMENTAL)
    test_ssl_tls1_3_derive_secret_wrapper,
#else
    NULL,
#endif
/* Function Id: 26 */

#if defined(MBEDTLS_SSL_TLS_C) && defined(MBEDTLS_SSL_PROTO_TLS1_3_EXPERIMENTAL)
    test_ssl_tls1_3_key_evolution_wrapper,
#else
    NULL,
#endif
/* Function Id: 27 */

#if defined(MBEDTLS_SSL_TLS_C)
    test_ssl_tls_prf_wrapper,
#else
    NULL,
#endif
/* Function Id: 28 */

#if defined(MBEDTLS_SSL_TLS_C)
    test_ssl_serialize_session_save_load_wrapper,
#else
    NULL,
#endif
/* Function Id: 29 */

#if defined(MBEDTLS_SSL_TLS_C)
    test_ssl_serialize_session_load_save_wrapper,
#else
    NULL,
#endif
/* Function Id: 30 */

#if defined(MBEDTLS_SSL_TLS_C)
    test_ssl_serialize_session_save_buf_size_wrapper,
#else
    NULL,
#endif
/* Function Id: 31 */

#if defined(MBEDTLS_SSL_TLS_C)
    test_ssl_serialize_session_load_buf_size_wrapper,
#else
    NULL,
#endif
/* Function Id: 32 */

#if defined(MBEDTLS_SSL_TLS_C)
    test_ssl_session_serialize_version_check_wrapper,
#else
    NULL,
#endif
/* Function Id: 33 */

#if defined(MBEDTLS_SSL_TLS_C) && defined(MBEDTLS_X509_CRT_PARSE_C) && defined(MBEDTLS_RSA_C) && defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED) && !defined(MBEDTLS_USE_PSA_CRYPTO) && defined(MBEDTLS_PKCS1_V15) && defined(MBEDTLS_ENTROPY_C) && defined(MBEDTLS_ENTROPY_C) && defined(MBEDTLS_CTR_DRBG_C)
    test_mbedtls_endpoint_sanity_wrapper,
#else
    NULL,
#endif
/* Function Id: 34 */

#if defined(MBEDTLS_SSL_TLS_C) && defined(MBEDTLS_X509_CRT_PARSE_C) && defined(MBEDTLS_RSA_C) && defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED) && !defined(MBEDTLS_USE_PSA_CRYPTO) && defined(MBEDTLS_PKCS1_V15) && defined(MBEDTLS_ENTROPY_C) && defined(MBEDTLS_ENTROPY_C) && defined(MBEDTLS_CTR_DRBG_C)
    test_move_handshake_to_state_wrapper,
#else
    NULL,
#endif
/* Function Id: 35 */

#if defined(MBEDTLS_SSL_TLS_C) && defined(MBEDTLS_X509_CRT_PARSE_C) && !defined(MBEDTLS_USE_PSA_CRYPTO) && defined(MBEDTLS_PKCS1_V15) && defined(MBEDTLS_RSA_C) && defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED) && defined(MBEDTLS_ENTROPY_C) && defined(MBEDTLS_CTR_DRBG_C)
    test_handshake_version_wrapper,
#else
    NULL,
#endif
/* Function Id: 36 */

#if defined(MBEDTLS_SSL_TLS_C) && defined(MBEDTLS_X509_CRT_PARSE_C) && !defined(MBEDTLS_USE_PSA_CRYPTO) && defined(MBEDTLS_PKCS1_V15) && defined(MBEDTLS_SSL_PROTO_TLS1_2) && defined(MBEDTLS_ENTROPY_C) && defined(MBEDTLS_CTR_DRBG_C)
    test_handshake_psk_cipher_wrapper,
#else
    NULL,
#endif
/* Function Id: 37 */

#if defined(MBEDTLS_SSL_TLS_C) && defined(MBEDTLS_X509_CRT_PARSE_C) && !defined(MBEDTLS_USE_PSA_CRYPTO) && defined(MBEDTLS_PKCS1_V15) && defined(MBEDTLS_SSL_PROTO_TLS1_2) && defined(MBEDTLS_ENTROPY_C) && defined(MBEDTLS_CTR_DRBG_C)
    test_handshake_cipher_wrapper,
#else
    NULL,
#endif
/* Function Id: 38 */

#if defined(MBEDTLS_SSL_TLS_C) && defined(MBEDTLS_X509_CRT_PARSE_C) && !defined(MBEDTLS_USE_PSA_CRYPTO) && defined(MBEDTLS_PKCS1_V15) && defined(MBEDTLS_SSL_PROTO_TLS1_2) && defined(MBEDTLS_RSA_C) && defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED) && defined(MBEDTLS_ENTROPY_C) && defined(MBEDTLS_CTR_DRBG_C)
    test_app_data_wrapper,
#else
    NULL,
#endif
/* Function Id: 39 */

#if defined(MBEDTLS_SSL_TLS_C) && defined(MBEDTLS_X509_CRT_PARSE_C) && !defined(MBEDTLS_USE_PSA_CRYPTO) && defined(MBEDTLS_PKCS1_V15) && defined(MBEDTLS_SSL_PROTO_TLS1_2) && defined(MBEDTLS_RSA_C) && defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED) && defined(MBEDTLS_ENTROPY_C) && defined(MBEDTLS_CTR_DRBG_C)
    test_app_data_tls_wrapper,
#else
    NULL,
#endif
/* Function Id: 40 */

#if defined(MBEDTLS_SSL_TLS_C) && defined(MBEDTLS_X509_CRT_PARSE_C) && !defined(MBEDTLS_USE_PSA_CRYPTO) && defined(MBEDTLS_PKCS1_V15) && defined(MBEDTLS_SSL_PROTO_TLS1_2) && defined(MBEDTLS_RSA_C) && defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED) && defined(MBEDTLS_SSL_PROTO_DTLS) && defined(MBEDTLS_ENTROPY_C) && defined(MBEDTLS_CTR_DRBG_C)
    test_app_data_dtls_wrapper,
#else
    NULL,
#endif
/* Function Id: 41 */

#if defined(MBEDTLS_SSL_TLS_C) && defined(MBEDTLS_X509_CRT_PARSE_C) && !defined(MBEDTLS_USE_PSA_CRYPTO) && defined(MBEDTLS_PKCS1_V15) && defined(MBEDTLS_SSL_PROTO_TLS1_2) && defined(MBEDTLS_RSA_C) && defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED) && defined(MBEDTLS_SSL_PROTO_DTLS) && defined(MBEDTLS_SSL_RENEGOTIATION) && defined(MBEDTLS_SSL_CONTEXT_SERIALIZATION) && defined(MBEDTLS_ENTROPY_C) && defined(MBEDTLS_CTR_DRBG_C)
    test_handshake_serialization_wrapper,
#else
    NULL,
#endif
/* Function Id: 42 */

#if defined(MBEDTLS_SSL_TLS_C) && defined(MBEDTLS_X509_CRT_PARSE_C) && !defined(MBEDTLS_USE_PSA_CRYPTO) && defined(MBEDTLS_PKCS1_V15) && defined(MBEDTLS_RSA_C) && defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED) && defined(MBEDTLS_DEBUG_C) && defined(MBEDTLS_SSL_MAX_FRAGMENT_LENGTH) && defined(MBEDTLS_CIPHER_MODE_CBC) && defined(MBEDTLS_ENTROPY_C) && defined(MBEDTLS_CTR_DRBG_C)
    test_handshake_fragmentation_wrapper,
#else
    NULL,
#endif
/* Function Id: 43 */

#if defined(MBEDTLS_SSL_TLS_C) && defined(MBEDTLS_X509_CRT_PARSE_C) && !defined(MBEDTLS_USE_PSA_CRYPTO) && defined(MBEDTLS_PKCS1_V15) && defined(MBEDTLS_SSL_PROTO_TLS1_2) && defined(MBEDTLS_RSA_C) && defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED) && defined(MBEDTLS_SSL_PROTO_DTLS) && defined(MBEDTLS_SSL_RENEGOTIATION) && defined(MBEDTLS_ENTROPY_C) && defined(MBEDTLS_CTR_DRBG_C)
    test_renegotiation_wrapper,
#else
    NULL,
#endif
/* Function Id: 44 */

#if defined(MBEDTLS_SSL_TLS_C) && defined(MBEDTLS_X509_CRT_PARSE_C) && !defined(MBEDTLS_USE_PSA_CRYPTO) && defined(MBEDTLS_PKCS1_V15) && defined(MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH) && defined(MBEDTLS_SSL_PROTO_TLS1_2) && defined(MBEDTLS_RSA_C) && defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED) && defined(MBEDTLS_ENTROPY_C) && defined(MBEDTLS_CTR_DRBG_C)
    test_resize_buffers_wrapper,
#else
    NULL,
#endif
/* Function Id: 45 */

#if defined(MBEDTLS_SSL_TLS_C) && defined(MBEDTLS_X509_CRT_PARSE_C) && !defined(MBEDTLS_USE_PSA_CRYPTO) && defined(MBEDTLS_PKCS1_V15) && defined(MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH) && defined(MBEDTLS_SSL_CONTEXT_SERIALIZATION) && defined(MBEDTLS_SSL_PROTO_TLS1_2) && defined(MBEDTLS_RSA_C) && defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED) && defined(MBEDTLS_SSL_PROTO_DTLS) && defined(MBEDTLS_ENTROPY_C) && defined(MBEDTLS_CTR_DRBG_C)
    test_resize_buffers_serialize_mfl_wrapper,
#else
    NULL,
#endif
/* Function Id: 46 */

#if defined(MBEDTLS_SSL_TLS_C) && defined(MBEDTLS_X509_CRT_PARSE_C) && !defined(MBEDTLS_USE_PSA_CRYPTO) && defined(MBEDTLS_PKCS1_V15) && defined(MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH) && defined(MBEDTLS_SSL_RENEGOTIATION) && defined(MBEDTLS_SSL_PROTO_TLS1_2) && defined(MBEDTLS_RSA_C) && defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED) && defined(MBEDTLS_ENTROPY_C) && defined(MBEDTLS_CTR_DRBG_C)
    test_resize_buffers_renegotiate_mfl_wrapper,
#else
    NULL,
#endif
/* Function Id: 47 */

#if defined(MBEDTLS_SSL_TLS_C) && defined(MBEDTLS_SSL_SOME_SUITES_USE_TLS_CBC) && defined(MBEDTLS_TEST_HOOKS)
    test_ssl_cf_hmac_wrapper,
#else
    NULL,
#endif
/* Function Id: 48 */

#if defined(MBEDTLS_SSL_TLS_C) && defined(MBEDTLS_SSL_SOME_SUITES_USE_TLS_CBC) && defined(MBEDTLS_TEST_HOOKS)
    test_ssl_cf_memcpy_offset_wrapper,
#else
    NULL,
#endif

};

/**
 * \brief        Execute the test function.
 *
 *               This is a wrapper function around the test function execution
 *               to allow the setjmp() call used to catch any calls to the
 *               parameter failure callback, to be used. Calls to setjmp()
 *               can invalidate the state of any local auto variables.
 *
 * \param fp     Function pointer to the test function.
 * \param params Parameters to pass to the #TestWrapper_t wrapper function.
 *
 */
void execute_function_ptr(TestWrapper_t fp, void **params)
{
#if defined(MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG)
    mbedtls_test_enable_insecure_external_rng( );
#endif

#if defined(MBEDTLS_CHECK_PARAMS)
    mbedtls_test_param_failed_location_record_t location_record;

    if ( setjmp( mbedtls_test_param_failed_get_state_buf( ) ) == 0 )
    {
        fp( params );
    }
    else
    {
        /* Unexpected parameter validation error */
        mbedtls_test_param_failed_get_location_record( &location_record );
        mbedtls_test_fail( location_record.failure_condition,
                           location_record.line,
                           location_record.file );
    }

    mbedtls_test_param_failed_reset_state( );
#else
    fp( params );
#endif

#if defined(MBEDTLS_TEST_MUTEX_USAGE)
    mbedtls_test_mutex_usage_check( );
#endif /* MBEDTLS_TEST_MUTEX_USAGE */
}

/**
 * \brief        Dispatches test functions based on function index.
 *
 * \param func_idx    Test function index.
 * \param params      The array of parameters to pass to the test function.
 *                    It will be decoded by the #TestWrapper_t wrapper function.
 *
 * \return       DISPATCH_TEST_SUCCESS if found
 *               DISPATCH_TEST_FN_NOT_FOUND if not found
 *               DISPATCH_UNSUPPORTED_SUITE if not compile time enabled.
 */
int dispatch_test( size_t func_idx, void ** params )
{
    int ret = DISPATCH_TEST_SUCCESS;
    TestWrapper_t fp = NULL;

    if ( func_idx < (int)( sizeof( test_funcs ) / sizeof( TestWrapper_t ) ) )
    {
        fp = test_funcs[func_idx];
        if ( fp )
            execute_function_ptr(fp, params);
        else
            ret = DISPATCH_UNSUPPORTED_SUITE;
    }
    else
    {
        ret = DISPATCH_TEST_FN_NOT_FOUND;
    }

    return( ret );
}


/**
 * \brief       Checks if test function is supported in this build-time
 *              configuration.
 *
 * \param func_idx    Test function index.
 *
 * \return       DISPATCH_TEST_SUCCESS if found
 *               DISPATCH_TEST_FN_NOT_FOUND if not found
 *               DISPATCH_UNSUPPORTED_SUITE if not compile time enabled.
 */
int check_test( size_t func_idx )
{
    int ret = DISPATCH_TEST_SUCCESS;
    TestWrapper_t fp = NULL;

    if ( func_idx < (int)( sizeof(test_funcs)/sizeof( TestWrapper_t ) ) )
    {
        fp = test_funcs[func_idx];
        if ( fp == NULL )
            ret = DISPATCH_UNSUPPORTED_SUITE;
    }
    else
    {
        ret = DISPATCH_TEST_FN_NOT_FOUND;
    }

    return( ret );
}

int main( int argc, const char *argv[] )
{
    int ret;
    mbedtls_test_platform_setup();
    ret = execute_tests( argc, argv, "/zip/third_party/mbedtls/test/test_suite_ssl.datax" );
    mbedtls_test_platform_teardown();
    return( ret );
}
