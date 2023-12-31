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
#include "libc/errno.h"
#include "libc/errno.h"
#include "libc/mem/mem.h"
#include "libc/sock/sock.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/inaddr.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/sock.h"
#include "libc/thread/thread.h"
#include "third_party/musl/lookup.internal.h"
#include "libc/intrin/atomic.h"
#include "third_party/musl/netdb.h"

asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2014 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");

int getaddrinfo(const char *host,
                const char *serv,
                const struct addrinfo *hint,
                struct addrinfo **res)
{
	struct service ports[MAXSERVS];
	struct address addrs[MAXADDRS];
	char canon[256], *outcanon;
	int nservs, naddrs, nais, canon_len, i, j, k;
	int family = AF_UNSPEC, flags = 0, proto = 0, socktype = 0;
	int no_family = 0;
	struct aibuf *out;

	if (!host && !serv) return EAI_NONAME;

	if (hint) {
		family = hint->ai_family;
		flags = hint->ai_flags;
		proto = hint->ai_protocol;
		socktype = hint->ai_socktype;

		const int mask = AI_PASSIVE | AI_CANONNAME | AI_NUMERICHOST |
			AI_V4MAPPED | AI_ALL | AI_ADDRCONFIG | AI_NUMERICSERV;
		if ((flags & mask) != flags)
			return EAI_BADFLAGS;

                if (family != AF_INET &&
                    family != AF_INET6 &&
                    family != AF_UNSPEC) {
			return EAI_FAMILY;
                }
	}

	if (flags & AI_ADDRCONFIG) {
		/* Define the "an address is configured" condition for address
		 * families via ability to create a socket for the family plus
		 * routability of the loopback address for the family. */
		const struct sockaddr_in lo4 = {
			.sin_family = AF_INET, .sin_port = 65535,
			.sin_addr.s_addr = __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
				? 0x7f000001 : 0x0100007f
		};
		const struct sockaddr_in6 lo6 = {
			.sin6_family = AF_INET6, .sin6_port = 65535,
			.sin6_addr = IN6ADDR_LOOPBACK_INIT
		};
		int tf[2] = { AF_INET, AF_INET6 };
		const void *ta[2] = { &lo4, &lo6 };
		socklen_t tl[2] = { sizeof lo4, sizeof lo6 };
		for (i=0; i<2; i++) {
			if (family==tf[1-i]) continue;
			int s = socket(tf[i], SOCK_CLOEXEC|SOCK_DGRAM,
				IPPROTO_UDP);
			if (s>=0) {
				int cs;
				pthread_setcancelstate(
					PTHREAD_CANCEL_DISABLE, &cs);
				int r = connect(s, ta[i], tl[i]);
				int saved_errno = errno;
				pthread_setcancelstate(cs, 0);
				close(s);
				if (!r) continue;
				errno = saved_errno;
			}
			if (errno != EADDRNOTAVAIL &&
			    errno != EAFNOSUPPORT &&
			    errno != EHOSTUNREACH &&
			    errno != ENETDOWN &&
			    errno != ENETUNREACH) {
				return EAI_SYSTEM;
			}
			if (family == tf[i]) no_family = 1;
			family = tf[1-i];
		}
	}

	nservs = __lookup_serv(ports, serv, proto, socktype, flags);
	if (nservs < 0) return nservs;

	naddrs = __lookup_name(addrs, canon, host, family, flags);
	if (naddrs < 0) return naddrs;

	if (no_family) return EAI_NODATA;

	nais = nservs * naddrs;
	canon_len = strlen(canon);
	out = calloc(1, nais * sizeof(*out) + canon_len + 1);
	if (!out) return EAI_MEMORY;

	if (canon_len) {
		outcanon = (void *)&out[nais];
		memcpy(outcanon, canon, canon_len+1);
	} else {
		outcanon = 0;
	}

	for (k=i=0; i<naddrs; i++) for (j=0; j<nservs; j++, k++) {
		out[k].slot = k;
		out[k].ai = (struct addrinfo){
			.ai_family = addrs[i].family,
			.ai_socktype = ports[j].socktype,
			.ai_protocol = ports[j].proto,
			.ai_addrlen = addrs[i].family == AF_INET
				? sizeof(struct sockaddr_in)
				: sizeof(struct sockaddr_in6),
			.ai_addr = (void *)&out[k].sa,
			.ai_canonname = outcanon };
		if (k) out[k-1].ai.ai_next = &out[k].ai;
		if (addrs[i].family == AF_INET) {
			out[k].sa.sin.sin_family = AF_INET;
			out[k].sa.sin.sin_port = htons(ports[j].port);
			memcpy(&out[k].sa.sin.sin_addr, &addrs[i].addr, 4);
		} else if (addrs[i].family == AF_INET6) {
			out[k].sa.sin6.sin6_family = AF_INET6;
			out[k].sa.sin6.sin6_port = htons(ports[j].port);
			out[k].sa.sin6.sin6_scope_id = addrs[i].scopeid;
			memcpy(&out[k].sa.sin6.sin6_addr, &addrs[i].addr, 16);
		}
	}
	atomic_store_explicit(&out[0].ref, nais, memory_order_relaxed);
	*res = &out->ai;
	return 0;
}
