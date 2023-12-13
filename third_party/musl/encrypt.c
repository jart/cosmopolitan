/*-*- mode:c;indent-tabs-mode:t;c-basic-offset:8;tab-width:8;coding:utf-8   -*-│
│ vi: set noet ft=c ts=8 sw=8 fenc=utf-8                                   :vi │
╚──────────────────────────────────────────────────────────────────────────────╝
│                                                                              │
│  Musl Libc                                                                   │
│  Copyright © 2005-2014 Rich Felker, et al.                                   │
│                                                                              │
│  Permission is hereby granted, free of charge, to any person obtaining       │
│  a copy of this software and associated documentation files (the             │
│  "Software"), to deal in the Software without restriction, including         │
│  without limitation the rights to use, copy, modify, merge, publish,         │
│  distribute, sublicense, and/or sell copies of the Software, and to          │
│  permit persons to whom the Software is furnished to do so, subject to       │
│  the following conditions:                                                   │
│                                                                              │
│  The above copyright notice and this permission notice shall be              │
│  included in all copies or substantial portions of the Software.             │
│                                                                              │
│  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,             │
│  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF          │
│  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.      │
│  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY        │
│  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,        │
│  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE           │
│  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                      │
│                                                                              │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/conv.h"
#include "libc/limits.h"
#include "libc/literal.h"
#include "libc/mem/alg.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/rand.h"
#include "libc/temp.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/ok.h"
#include "third_party/gdtoa/gdtoa.h"
#include "third_party/musl/crypt_des.internal.h"

asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2014 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");

static struct expanded_key __encrypt_key;

void setkey(const char *key)
{
	unsigned char bkey[8];
	int i, j;

	for (i = 0; i < 8; i++) {
		bkey[i] = 0;
		for (j = 7; j >= 0; j--, key++)
			bkey[i] |= (uint32_t)(*key & 1) << j;
	}

	__des_setkey(bkey, &__encrypt_key);
}

void encrypt(char *block, int edflag)
{
	struct expanded_key decrypt_key, *key;
	uint32_t b[2];
	int i, j;
	char *p;

	p = block;
	for (i = 0; i < 2; i++) {
		b[i] = 0;
		for (j = 31; j >= 0; j--, p++)
			b[i] |= (uint32_t)(*p & 1) << j;
	}

	key = &__encrypt_key;
	if (edflag) {
		key = &decrypt_key;
		for (i = 0; i < 16; i++) {
			decrypt_key.l[i] = __encrypt_key.l[15-i];
			decrypt_key.r[i] = __encrypt_key.r[15-i];
		}
	}

	__do_des(b[0], b[1], b, b + 1, 1, 0, key);

	p = block;
	for (i = 0; i < 2; i++)
		for (j = 31; j >= 0; j--)
			*p++ = b[i]>>j & 1;
}
