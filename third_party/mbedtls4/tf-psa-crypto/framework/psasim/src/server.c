/* psasim test server */

/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#include <unistd.h>
#include <stdio.h>

/* Includes from psasim */
#include "service.h"
#include "error_ext.h"
#include "util.h"
#include "psa_manifest/manifest.h"
#include "psa_functions_codes.h"

/* Includes from mbedtls */
#include "mbedtls/version.h"
#include "psa/crypto.h"

#ifdef DEBUG
#define SERVER_PRINT(fmt, ...) \
    PRINT("Server: " fmt, ##__VA_ARGS__)
#else
#define SERVER_PRINT(...)
#endif

#define BUF_SIZE 25

static int kill_on_disconnect = 0; /* Kill the server on client disconnection. */

void parse_input_args(int argc, char *argv[])
{
    int opt;

    while ((opt = getopt(argc, argv, "k")) != -1) {
        switch (opt) {
            case 'k':
                kill_on_disconnect = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [-k]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
}

int psa_server_main(int argc, char *argv[])
{
    psa_status_t ret = PSA_ERROR_PROGRAMMER_ERROR;
    psa_msg_t msg = { -1 };
    const int magic_num = 66;
    int client_disconnected = 0;
    extern psa_status_t psa_crypto_call(psa_msg_t msg);
    extern psa_status_t psa_crypto_close(void);

#if defined(MBEDTLS_VERSION_C)
    const char *mbedtls_version = mbedtls_version_get_string_full();
    SERVER_PRINT("%s", mbedtls_version);
#endif

    parse_input_args(argc, argv);
    SERVER_PRINT("Starting");

    while (!(kill_on_disconnect && client_disconnected)) {
        psa_signal_t signals = psa_wait(PSA_WAIT_ANY, PSA_BLOCK);

        if (signals > 0) {
            SERVER_PRINT("Signals: 0x%08x", signals);
        }

        if (signals & PSA_CRYPTO_SIGNAL) {
            if (PSA_SUCCESS == psa_get(PSA_CRYPTO_SIGNAL, &msg)) {
                SERVER_PRINT("handle: %d - rhandle: %p", msg.handle, (int *) msg.rhandle);
                switch (msg.type) {
                    case PSA_IPC_CONNECT:
                        SERVER_PRINT("Got a connection message");
                        psa_set_rhandle(msg.handle, (void *) &magic_num);
                        ret = PSA_SUCCESS;
                        break;
                    case PSA_IPC_DISCONNECT:
                        SERVER_PRINT("Got a disconnection message");
                        ret = PSA_SUCCESS;
                        client_disconnected = 1;
                        psa_crypto_close();
                        break;
                    default:
                        SERVER_PRINT("Got an IPC call of type %d", msg.type);
                        ret = psa_crypto_call(msg);
                        SERVER_PRINT("Internal function call returned %d", ret);

                        if (msg.client_id > 0) {
                            psa_notify(msg.client_id);
                        } else {
                            SERVER_PRINT("Client is non-secure, so won't notify");
                        }
                }

                psa_reply(msg.handle, ret);
            } else {
                SERVER_PRINT("Failed to retrieve message");
            }
        } else if (SIGSTP_SIG & signals) {
            SERVER_PRINT("Recieved SIGSTP signal. Gonna EOI it.");
            psa_eoi(SIGSTP_SIG);
        } else if (SIGINT_SIG & signals) {
            SERVER_PRINT("Handling interrupt!");
            SERVER_PRINT("Gracefully quitting");
            psa_panic();
        } else {
            SERVER_PRINT("No signal asserted");
        }
    }

    return 0;
}
