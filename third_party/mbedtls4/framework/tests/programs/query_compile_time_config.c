/*
 *  Query the Mbed TLS compile time configuration
 *
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#include "mbedtls/build_info.h"

#include "mbedtls/platform.h"

#define USAGE                                                                   \
    "usage: %s [ -all | -any | -l ] <MBEDTLS_CONFIG> ...\n\n"                   \
    "This program takes command line arguments which correspond to\n"           \
    "the string representation of Mbed TLS compile time configurations.\n\n"    \
    "If \"--all\" and \"--any\" are not used, then, if all given arguments\n"   \
    "are defined in the Mbed TLS build, 0 is returned; otherwise 1 is\n"        \
    "returned. Macro expansions of configurations will be printed (if any).\n"                                 \
    "-l\tPrint all available configuration.\n"                                  \
    "-all\tReturn 0 if all configurations are defined. Otherwise, return 1\n"   \
    "-any\tReturn 0 if any configuration is defined. Otherwise, return 1\n"     \
    "-h\tPrint this usage\n"

#include <string.h>
#include "query_config.h"

int main(int argc, char *argv[])
{
    int i;

    if (argc < 2 || strcmp(argv[1], "-h") == 0) {
        mbedtls_printf(USAGE, argv[0]);
        return MBEDTLS_EXIT_FAILURE;
    }

    if (strcmp(argv[1], "-l") == 0) {
        list_config();
        return 0;
    }

    if (strcmp(argv[1], "-all") == 0) {
        for (i = 2; i < argc; i++) {
            if (query_config(argv[i]) != 0) {
                return 1;
            }
        }
        return 0;
    }

    if (strcmp(argv[1], "-any") == 0) {
        for (i = 2; i < argc; i++) {
            if (query_config(argv[i]) == 0) {
                return 0;
            }
        }
        return 1;
    }

    for (i = 1; i < argc; i++) {
        if (query_config(argv[i]) != 0) {
            return 1;
        }
    }

    return 0;
}
