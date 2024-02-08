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
#include "libc/errno.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "third_party/musl/lookup.internal.h"
#include "third_party/musl/netdb.h"

asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2014 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");

errno_t gethostbyname2_r(const char *name, int af,
	struct hostent *h, char *buf, size_t buflen,
	struct hostent **res, int *err)
{
	struct address addrs[MAXADDRS];
	char canon[256];
	int i, cnt;
	size_t align, need;

	*res = 0;
	cnt = __lookup_name(addrs, canon, name, af, AI_CANONNAME);
	if (cnt<0) switch (cnt) {
	case EAI_NONAME:
		*err = HOST_NOT_FOUND;
		return 0;
	case EAI_NODATA:
		*err = NO_DATA;
		return 0;
	case EAI_AGAIN:
		*err = TRY_AGAIN;
		return EAGAIN;
	default:
	case EAI_FAIL:
		*err = NO_RECOVERY;
		return EBADMSG;
	case EAI_SYSTEM:
		*err = NO_RECOVERY;
		return errno;
	}

	h->h_addrtype = af;
	h->h_length = af==AF_INET6 ? 16 : 4;

	/* Align buffer */
	align = -(uintptr_t)buf & (sizeof(char *)-1);

	need = 4*sizeof(char *);
	need += (cnt + 1) * (sizeof(char *) + h->h_length);
	need += strlen(name)+1;
	need += strlen(canon)+1;
	need += align;

	if (need > buflen) return ERANGE;

	buf += align;
	h->h_aliases = (void *)buf;
	buf += 3*sizeof(char *);
	h->h_addr_list = (void *)buf;
	buf += (cnt+1)*sizeof(char *);

	for (i=0; i<cnt; i++) {
		h->h_addr_list[i] = (void *)buf;
		buf += h->h_length;
		memcpy(h->h_addr_list[i], addrs[i].addr, h->h_length);
	}
	h->h_addr_list[i] = 0;

	h->h_name = h->h_aliases[0] = buf;
	strcpy(h->h_name, canon);
	buf += strlen(h->h_name)+1;

	if (strcmp(h->h_name, name)) {
		h->h_aliases[1] = buf;
		strcpy(h->h_aliases[1], name);
		buf += strlen(h->h_aliases[1])+1;
	} else h->h_aliases[1] = 0;

	h->h_aliases[2] = 0;

	*res = h;
	return 0;
}
