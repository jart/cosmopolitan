/* PSA status codes used by psasim. */

/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#ifndef PSA_ERROR_H
#define PSA_ERROR_H

#include <stdint.h>

#include "common.h"

#define PSA_ERROR_PROGRAMMER_ERROR      ((psa_status_t) -129)
#define PSA_ERROR_CONNECTION_REFUSED    ((psa_status_t) -130)
#define PSA_ERROR_CONNECTION_BUSY       ((psa_status_t) -131)

#endif
