/*-*- mode:c;indent-tabs-mode:t;c-basic-offset:8;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=8 sts=8 sw=8 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Cosmopolitan translation unit for SQLCipher (SQLite 3.53.4 amalgamation      │
│ plus the mbedtls crypto provider).                                           │
╚─────────────────────────────────────────────────────────────────────────────*/
__notice(sqlcipher_notice, "\
SQLCipher (BSD-3-Clause)\n\
Copyright (c) 2008-2026, ZETETIC LLC\n\
SQLite (Public Domain)\n\
The author disclaims copyright to the SQLite source");

#include "libc/stdio/rand.h"
#include "libc/str/str.h"
#include "third_party/mbedtls/aes.h"
#include "third_party/mbedtls/md.h"
#include "third_party/mbedtls/pkcs5.h"
#include "third_party/mbedtls/version.h"
#include "third_party/sqlcipher/sqlite3.c"
#include "third_party/sqlcipher/crypto_mbedtls.c"
