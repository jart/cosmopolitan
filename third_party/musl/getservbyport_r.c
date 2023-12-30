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
#include "libc/fmt/conv.h"
#include "libc/sock/sock.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "third_party/musl/netdb.h"

asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2014 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");

errno_t getservbyport_r(int port, const char *prots,
	struct servent *se, char *buf, size_t buflen, struct servent **res)
{
	int i;
	struct sockaddr_in sin = {
		.sin_family = AF_INET,
		.sin_port = port,
	};

	if (!prots) {
		int r = getservbyport_r(port, "tcp", se, buf, buflen, res);
		if (r) r = getservbyport_r(port, "udp", se, buf, buflen, res);
		return r;
	}
	*res = 0;

	/* Align buffer */
	i = (uintptr_t)buf & (sizeof(char *)-1);
	if (!i) i = sizeof(char *);
	if (buflen <= 3*sizeof(char *)-i)
		return ERANGE;
	buf += sizeof(char *)-i;
	buflen -= sizeof(char *)-i;

	if (strcmp(prots, "tcp") && strcmp(prots, "udp")) return EINVAL;

	se->s_port = port;
	se->s_proto = (char *)prots;
	se->s_aliases = (void *)buf;
	buf += 2*sizeof(char *);
	buflen -= 2*sizeof(char *);
	se->s_aliases[1] = 0;
	se->s_aliases[0] = se->s_name = buf;

	switch (getnameinfo((void *)&sin, sizeof sin, 0, 0, buf, buflen,
		strcmp(prots, "udp") ? 0 : NI_DGRAM)) {
	case EAI_MEMORY:
	case EAI_SYSTEM:
		return ENOMEM;
	case EAI_OVERFLOW:
		return ERANGE;
	default:
		return ENOENT;
	case 0:
		break;
	}

	/* A numeric port string is not a service record. */
	if (strtol(buf, 0, 10)==ntohs(port)) return ENOENT;

	*res = se;
	return 0;
}
