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
#include "libc/calls/weirdtypes.h"
#include "libc/errno.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/sock/struct/sockaddr6.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "third_party/musl/netdb.h"

asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2014 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");

errno_t gethostbyaddr_r(const void *a, socklen_t l, int af,
	struct hostent *h, char *buf, size_t buflen,
	struct hostent **res, int *err)
{
	union {
		struct sockaddr_in sin;
		struct sockaddr_in6 sin6;
	} sa = { .sin.sin_family = af };
	socklen_t sl = af==AF_INET6 ? sizeof sa.sin6 : sizeof sa.sin;
	int i;

	*res = 0;

	/* Load address argument into sockaddr structure */
	if (af==AF_INET6 && l==16) memcpy(&sa.sin6.sin6_addr, a, 16);
	else if (af==AF_INET && l==4) memcpy(&sa.sin.sin_addr, a, 4);
	else {
		*err = NO_RECOVERY;
		return EINVAL;
	}

	/* Align buffer and check for space for pointers and ip address */
	i = (uintptr_t)buf & (sizeof(char *)-1);
	if (!i) i = sizeof(char *);
	if (buflen <= 5*sizeof(char *)-i + l) return ERANGE;
	buf += sizeof(char *)-i;
	buflen -= 5*sizeof(char *)-i + l;

	h->h_addr_list = (void *)buf;
	buf += 2*sizeof(char *);
	h->h_aliases = (void *)buf;
	buf += 2*sizeof(char *);

	h->h_addr_list[0] = buf;
	memcpy(h->h_addr_list[0], a, l);
	buf += l;
	h->h_addr_list[1] = 0;
	h->h_aliases[0] = buf;
	h->h_aliases[1] = 0;

	switch (getnameinfo((void *)&sa, sl, buf, buflen, 0, 0, 0)) {
	case EAI_AGAIN:
		*err = TRY_AGAIN;
		return EAGAIN;
	case EAI_OVERFLOW:
		return ERANGE;
	default:
	case EAI_FAIL:
		*err = NO_RECOVERY;
		return EBADMSG;
	case EAI_SYSTEM:
		*err = NO_RECOVERY;
		return errno;
	case 0:
		break;
	}

	h->h_addrtype = af;
	h->h_length = l;
	h->h_name = h->h_aliases[0];
	*res = h;
	return 0;
}
