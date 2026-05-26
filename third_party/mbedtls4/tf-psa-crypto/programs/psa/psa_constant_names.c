/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "psa/crypto.h"

/* This block is present to support Visual Studio builds prior to 2015 */
#if defined(_MSC_VER) && _MSC_VER < 1900
#include <stdarg.h>
int snprintf(char *s, size_t n, const char *fmt, ...)
{
    int ret;
    va_list argp;

    /* Avoid calling the invalid parameter handler by checking ourselves */
    if (s == NULL || n == 0 || fmt == NULL) {
        return -1;
    }

    va_start(argp, fmt);
#if defined(_TRUNCATE) && !defined(__MINGW32__)
    ret = _vsnprintf_s(s, n, _TRUNCATE, fmt, argp);
#else
    ret = _vsnprintf(s, n, fmt, argp);
    if (ret < 0 || (size_t) ret == n) {
        s[n-1] = '\0';
        ret = -1;
    }
#endif
    va_end(argp);

    return ret;
}
#endif

static void append(char **buffer, size_t buffer_size,
                   size_t *required_size,
                   const char *string, size_t length)
{
    *required_size += length;
    if (*required_size < buffer_size) {
        memcpy(*buffer, string, length);
        *buffer += length;
    }
}

static void append_integer(char **buffer, size_t buffer_size,
                           size_t *required_size,
                           const char *format /*printf format for value*/,
                           unsigned long value)
{
    size_t n = snprintf(*buffer, buffer_size - *required_size, format, value);
    if (n < buffer_size - *required_size) {
        *buffer += n;
    }
    *required_size += n;
}

/* The code of these function is automatically generated and included below. */
static const char *psa_ecc_family_name(psa_ecc_family_t curve);
static const char *psa_dh_family_name(psa_dh_family_t group);
static const char *psa_hash_algorithm_name(psa_algorithm_t hash_alg);

static void append_with_curve(char **buffer, size_t buffer_size,
                              size_t *required_size,
                              const char *string, size_t length,
                              psa_ecc_family_t curve)
{
    const char *family_name = psa_ecc_family_name(curve);
    append(buffer, buffer_size, required_size, string, length);
    append(buffer, buffer_size, required_size, "(", 1);
    if (family_name != NULL) {
        append(buffer, buffer_size, required_size,
               family_name, strlen(family_name));
    } else {
        append_integer(buffer, buffer_size, required_size,
                       "0x%02x", curve);
    }
    append(buffer, buffer_size, required_size, ")", 1);
}

static void append_with_group(char **buffer, size_t buffer_size,
                              size_t *required_size,
                              const char *string, size_t length,
                              psa_dh_family_t group)
{
    const char *group_name = psa_dh_family_name(group);
    append(buffer, buffer_size, required_size, string, length);
    append(buffer, buffer_size, required_size, "(", 1);
    if (group_name != NULL) {
        append(buffer, buffer_size, required_size,
               group_name, strlen(group_name));
    } else {
        append_integer(buffer, buffer_size, required_size,
                       "0x%02x", group);
    }
    append(buffer, buffer_size, required_size, ")", 1);
}

typedef const char *(*psa_get_algorithm_name_func_ptr)(psa_algorithm_t alg);

static void append_with_alg(char **buffer, size_t buffer_size,
                            size_t *required_size,
                            psa_get_algorithm_name_func_ptr get_name,
                            psa_algorithm_t alg)
{
    const char *name = get_name(alg);
    if (name != NULL) {
        append(buffer, buffer_size, required_size,
               name, strlen(name));
    } else {
        append_integer(buffer, buffer_size, required_size,
                       "0x%08lx", alg);
    }
}

#include "psa_constant_names_generated.c"

static int psa_snprint_status(char *buffer, size_t buffer_size,
                              psa_status_t status)
{
    const char *name = psa_strerror(status);
    if (name == NULL) {
        return snprintf(buffer, buffer_size, "%ld", (long) status);
    } else {
        size_t length = strlen(name);
        if (length < buffer_size) {
            memcpy(buffer, name, length + 1);
            return (int) length;
        } else {
            return (int) buffer_size;
        }
    }
}

static int psa_snprint_ecc_curve(char *buffer, size_t buffer_size,
                                 psa_ecc_family_t curve)
{
    const char *name = psa_ecc_family_name(curve);
    if (name == NULL) {
        return snprintf(buffer, buffer_size, "0x%02x", (unsigned) curve);
    } else {
        size_t length = strlen(name);
        if (length < buffer_size) {
            memcpy(buffer, name, length + 1);
            return (int) length;
        } else {
            return (int) buffer_size;
        }
    }
}

static int psa_snprint_dh_group(char *buffer, size_t buffer_size,
                                psa_dh_family_t group)
{
    const char *name = psa_dh_family_name(group);
    if (name == NULL) {
        return snprintf(buffer, buffer_size, "0x%02x", (unsigned) group);
    } else {
        size_t length = strlen(name);
        if (length < buffer_size) {
            memcpy(buffer, name, length + 1);
            return (int) length;
        } else {
            return (int) buffer_size;
        }
    }
}

static void usage(const char *program_name)
{
    printf("Usage: %s TYPE VALUE [VALUE...]\n",
           program_name == NULL ? "psa_constant_names" : program_name);
    printf("Print the symbolic name whose numerical value is VALUE in TYPE.\n");
    printf("Supported types (with = between aliases):\n");
    printf("  alg=algorithm         Algorithm (psa_algorithm_t)\n");
    printf("  curve=ecc_curve       Elliptic curve identifier (psa_ecc_family_t)\n");
    printf("  group=dh_group        Diffie-Hellman group identifier (psa_dh_family_t)\n");
    printf("  type=key_type         Key type (psa_key_type_t)\n");
    printf("  usage=key_usage       Key usage (psa_key_usage_t)\n");
    printf("  error=status          Status code (psa_status_t)\n");
}

typedef enum {
    TYPE_STATUS,
} signed_value_type;

static int process_signed(signed_value_type type, long min, long max, char **argp)
{
    for (; *argp != NULL; argp++) {
        char buffer[200];
        char *end;
        long value = strtol(*argp, &end, 0);
        if (*end) {
            printf("Non-numeric value: %s\n", *argp);
            return EXIT_FAILURE;
        }
        if (value < min || (errno == ERANGE && value < 0)) {
            printf("Value too small: %s\n", *argp);
            return EXIT_FAILURE;
        }
        if (value > max || (errno == ERANGE && value > 0)) {
            printf("Value too large: %s\n", *argp);
            return EXIT_FAILURE;
        }

        switch (type) {
            case TYPE_STATUS:
                psa_snprint_status(buffer, sizeof(buffer),
                                   (psa_status_t) value);
                break;
        }
        puts(buffer);
    }

    return EXIT_SUCCESS;
}

typedef enum {
    TYPE_ALGORITHM,
    TYPE_ECC_CURVE,
    TYPE_DH_GROUP,
    TYPE_KEY_TYPE,
    TYPE_KEY_USAGE,
} unsigned_value_type;

static int process_unsigned(unsigned_value_type type, unsigned long max, char **argp)
{
    for (; *argp != NULL; argp++) {
        char buffer[200];
        char *end;
        unsigned long value = strtoul(*argp, &end, 0);
        if (*end) {
            printf("Non-numeric value: %s\n", *argp);
            return EXIT_FAILURE;
        }
        if (value > max || errno == ERANGE) {
            printf("Value out of range: %s\n", *argp);
            return EXIT_FAILURE;
        }

        switch (type) {
            case TYPE_ALGORITHM:
                psa_snprint_algorithm(buffer, sizeof(buffer),
                                      (psa_algorithm_t) value);
                break;
            case TYPE_ECC_CURVE:
                psa_snprint_ecc_curve(buffer, sizeof(buffer),
                                      (psa_ecc_family_t) value);
                break;
            case TYPE_DH_GROUP:
                psa_snprint_dh_group(buffer, sizeof(buffer),
                                     (psa_dh_family_t) value);
                break;
            case TYPE_KEY_TYPE:
                psa_snprint_key_type(buffer, sizeof(buffer),
                                     (psa_key_type_t) value);
                break;
            case TYPE_KEY_USAGE:
                psa_snprint_key_usage(buffer, sizeof(buffer),
                                      (psa_key_usage_t) value);
                break;
        }
        puts(buffer);
    }

    return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
    if (argc <= 1 ||
        !strcmp(argv[1], "help") ||
        !strcmp(argv[1], "--help")) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    if (!strcmp(argv[1], "error") || !strcmp(argv[1], "status")) {
        /* There's no way to obtain the actual range of a signed type,
         * so hard-code it here: psa_status_t is int32_t. */
        return process_signed(TYPE_STATUS, INT32_MIN, INT32_MAX,
                              argv + 2);
    } else if (!strcmp(argv[1], "alg") || !strcmp(argv[1], "algorithm")) {
        return process_unsigned(TYPE_ALGORITHM, (psa_algorithm_t) (-1),
                                argv + 2);
    } else if (!strcmp(argv[1], "curve") || !strcmp(argv[1], "ecc_curve")) {
        return process_unsigned(TYPE_ECC_CURVE, (psa_ecc_family_t) (-1),
                                argv + 2);
    } else if (!strcmp(argv[1], "group") || !strcmp(argv[1], "dh_group")) {
        return process_unsigned(TYPE_DH_GROUP, (psa_dh_family_t) (-1),
                                argv + 2);
    } else if (!strcmp(argv[1], "type") || !strcmp(argv[1], "key_type")) {
        return process_unsigned(TYPE_KEY_TYPE, (psa_key_type_t) (-1),
                                argv + 2);
    } else if (!strcmp(argv[1], "usage") || !strcmp(argv[1], "key_usage")) {
        return process_unsigned(TYPE_KEY_USAGE, (psa_key_usage_t) (-1),
                                argv + 2);
    } else {
        printf("Unknown type: %s\n", argv[1]);
        return EXIT_FAILURE;
    }
}
