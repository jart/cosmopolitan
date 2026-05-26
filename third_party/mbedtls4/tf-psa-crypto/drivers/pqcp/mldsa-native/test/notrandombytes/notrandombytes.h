/*
 * Copyright (c) The mldsa-native project authors
 * SPDX-License-Identifier: LicenseRef-PD-hp OR CC0-1.0 OR 0BSD OR MIT-0 OR MIT
 */

/* References
 * ==========
 *
 * - [surf]
 *   SURF: Simple Unpredictable Random Function
 *   Daniel J. Bernstein
 *   https://cr.yp.to/papers.html#surf
 */

/* Based on @[surf]. */

#ifndef NOTRANDOMBYTES_H
#define NOTRANDOMBYTES_H

#include <stdint.h>
#include <stdlib.h>

/**
 * WARNING
 *
 * The randombytes() implementation in this file is for TESTING ONLY.
 * You MUST NOT use this implementation outside of testing.
 *
 */

void randombytes_reset(void);
int randombytes(uint8_t *buf, size_t n);

#endif /* !NOTRANDOMBYTES_H */
