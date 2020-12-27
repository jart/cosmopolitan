/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/bits/bits.h"
#include "libc/bits/progn.internal.h"
#include "libc/bits/safemacros.h"
#include "libc/fmt/fmt.h"
#include "libc/testlib/testlib.h"

static char buffer[128];

#define Format(...) PROGN(snprintf(buffer, sizeof(buffer), __VA_ARGS__), buffer)

/**
 * @fileoverview String formatting tests.
 *
 * We use textual includes here to test UTF-8, UTF-16, and UTF-32 at the
 * same time, since Cosmopolitan is designed to support them all without
 * conditions.
 */

#define SUITE(NAME) NAME##s
#define FORMAT(STR) STR "s"
#define STRING(STR) STR
#include "test/libc/fmt/sprintf_s.inc"
#undef SUITE
#undef FORMAT
#undef STRING

#define SUITE(NAME) NAME##hs
#define FORMAT(STR) STR "hs"
#define STRING(STR) u##STR
#include "test/libc/fmt/sprintf_s.inc"
#undef SUITE
#undef FORMAT
#undef STRING

#define SUITE(NAME) NAME##ls
#define FORMAT(STR) STR "ls"
#define STRING(STR) L##STR
#include "test/libc/fmt/sprintf_s.inc"
#undef SUITE
#undef FORMAT
#undef STRING
