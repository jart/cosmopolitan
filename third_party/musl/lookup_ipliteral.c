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
#include "libc/sock/struct/sockaddr.h"
#include "libc/sysv/consts/af.h"
#include "libc/str/str.h"
#include "libc/fmt/conv.h"
#include "libc/limits.h"
#include "libc/sock/sock.h"
#include "third_party/musl/lookup.internal.h"

asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2014 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");

int __lookup_ipliteral(struct address buf[static 1], const char *name, int family)
{
	struct in_addr a4;
	struct in6_addr a6;
	if (inet_aton(name, &a4) > 0) {
		if (family == AF_INET6) /* wrong family */
			return EAI_NODATA;
		memcpy(&buf[0].addr, &a4, sizeof a4);
		buf[0].family = AF_INET;
		buf[0].scopeid = 0;
		return 1;
	}

	char tmp[64];
	char *p = strchr(name, '%'), *z;
	unsigned long long scopeid = 0;
	if (p && p-name < 64) {
		memcpy(tmp, name, p-name);
		tmp[p-name] = 0;
		name = tmp;
	}

	if (inet_pton(AF_INET6, name, &a6) <= 0)
		return 0;
	if (family == AF_INET) /* wrong family */
		return EAI_NODATA;

	memcpy(&buf[0].addr, &a6, sizeof a6);
	buf[0].family = AF_INET6;
	if (p) {
		if (isdigit(*++p)) scopeid = strtoull(p, &z, 10);
		else z = p-1;
		if (*z) {
#if 1
			// todo(jart): do something
			return EAI_NONAME;
#else
			if (!IN6_IS_ADDR_LINKLOCAL(&a6) &&
			    !IN6_IS_ADDR_MC_LINKLOCAL(&a6))
				return EAI_NONAME;
			scopeid = if_nametoindex(p);
			if (!scopeid)
				return EAI_NONAME;
#endif
		}
		if (scopeid > UINT_MAX)
			return EAI_NONAME;
	}
	buf[0].scopeid = scopeid;
	return 1;
}
