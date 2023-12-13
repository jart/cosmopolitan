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
#include "third_party/musl/crypt.h"
#include "third_party/musl/crypt.internal.h"

asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2014 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");

/**
 * Encrypts password the old fashioned way.
 *
 * The method of encryption depends on the first three chars of salt:
 *
 * - `$1$` is MD5
 * - `$2$` is Blowfish
 * - `$5$` is SHA-256
 * - `$6$` is SHA-512
 * - Otherwise DES
 *
 * @return static memory with encrypted password
 * @see third_party/argon2/
 */
char *crypt_r(const char *key, const char *salt, struct crypt_data *data)
{
	/* Per the crypt_r API, the caller has provided a pointer to
	 * struct crypt_data; however, this implementation does not
	 * use the structure to store any internal state, and treats
	 * it purely as a char buffer for storing the result. */
	char *output = (char *)data;
	if (salt[0] == '$' && salt[1] && salt[2]) {
		if (salt[1] == '1' && salt[2] == '$')
			return __crypt_md5(key, salt, output);
		if (salt[1] == '2' && salt[3] == '$')
			return __crypt_blowfish(key, salt, output);
		if (salt[1] == '5' && salt[2] == '$')
			return __crypt_sha256(key, salt, output);
		if (salt[1] == '6' && salt[2] == '$')
			return __crypt_sha512(key, salt, output);
	}
	return __crypt_des(key, salt, output);
}
