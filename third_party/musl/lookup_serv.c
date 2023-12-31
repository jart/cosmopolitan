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
#include "libc/sysv/consts/sock.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/fmt/conv.h"
#include "libc/str/str.h"
#include "libc/stdio/internal.h"
#include "libc/errno.h"
#include "libc/calls/sysdir.internal.h"
#include "third_party/musl/lookup.internal.h"

asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2014 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");

int __lookup_serv(struct service buf[static MAXSERVS],
		  const char *name,
		  int proto,
		  int socktype,
		  int flags)
{
	char line[128];
	int cnt = 0;
	char *p, *z = "";
	unsigned long port = 0;

	switch (socktype) {
	case SOCK_STREAM:
		switch (proto) {
		case 0:
			proto = IPPROTO_TCP;
		case IPPROTO_TCP:
			break;
		default:
			return EAI_SERVICE;
		}
		break;
	case SOCK_DGRAM:
		switch (proto) {
		case 0:
			proto = IPPROTO_UDP;
		case IPPROTO_UDP:
			break;
		default:
			return EAI_SERVICE;
		}
	case 0:
		break;
	default:
		if (name) return EAI_SERVICE;
		buf[0].port = 0;
		buf[0].proto = proto;
		buf[0].socktype = socktype;
		return 1;
	}

	if (name) {
		if (!*name) return EAI_SERVICE;
		port = strtoul(name, &z, 10);
	}
	if (!*z) {
		if (port > 65535) return EAI_SERVICE;
		if (proto != IPPROTO_UDP) {
			buf[cnt].port = port;
			buf[cnt].socktype = SOCK_STREAM;
			buf[cnt++].proto = IPPROTO_TCP;
		}
		if (proto != IPPROTO_TCP) {
			buf[cnt].port = port;
			buf[cnt].socktype = SOCK_DGRAM;
			buf[cnt++].proto = IPPROTO_UDP;
		}
		return cnt;
	}

	if (flags & AI_NUMERICSERV) return EAI_NONAME;

	size_t l = strlen(name);

	/* get path of /etc/services */
	char etc_services_buf[256];
	const char *etc_services_path =
		GetServicesTxtPath(etc_services_buf,
				   sizeof(etc_services_buf));

	/* open services definitions file */
	FILE *f = fopen(etc_services_path, "rbe");
	if (!f) switch (errno) {
	case ENOENT:
	case ENOTDIR:
	case EACCES:
		return EAI_SERVICE;
	default:
		return EAI_SYSTEM;
	}

	while (fgets(line, sizeof line, f) && cnt < MAXSERVS) {
		if ((p=strchr(line, '#'))) *p++='\n', *p=0;

		/* Find service name */
		for(p=line; (p=strstr(p, name)); p++) {
			if (p>line && !isspace(p[-1])) continue;
			if (p[l] && !isspace(p[l])) continue;
			break;
		}
		if (!p) continue;

		/* Skip past canonical name at beginning of line */
		for (p=line; *p && !isspace(*p); p++);

		port = strtoul(p, &z, 10);
		if (port > 65535 || z==p) continue;
		if (!strncmp(z, "/udp", 4)) {
			if (proto == IPPROTO_TCP) continue;
			buf[cnt].port = port;
			buf[cnt].socktype = SOCK_DGRAM;
			buf[cnt++].proto = IPPROTO_UDP;
		}
		if (!strncmp(z, "/tcp", 4)) {
			if (proto == IPPROTO_UDP) continue;
			buf[cnt].port = port;
			buf[cnt].socktype = SOCK_STREAM;
			buf[cnt++].proto = IPPROTO_TCP;
		}
	}
	fclose(f);
	return cnt > 0 ? cnt : EAI_SERVICE;
}
