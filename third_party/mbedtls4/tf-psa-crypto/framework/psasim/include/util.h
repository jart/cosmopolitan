/* Common definitions used for clients and services */

/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#include "service.h"

#include <stdio.h>

#define PRINT(fmt, ...) \
    fprintf(stdout, fmt "\n", ##__VA_ARGS__)

#if defined(DEBUG)
#define INFO(fmt, ...) \
    fprintf(stdout, "Info (%s - %d): " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else /* !DEBUG */
#define INFO(...)
#endif /* DEBUG*/

#define ERROR(fmt, ...) \
    fprintf(stderr, "Error (%s - %d): " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define FATAL(fmt, ...) \
    { \
        fprintf(stderr, "Fatal (%s - %d): " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
        abort(); \
    }

#define PROJECT_ID              'M'
#define PATHNAMESIZE            256
#define TMP_FILE_BASE_PATH      "./"
