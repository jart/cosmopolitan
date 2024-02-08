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
#include "libc/calls/sysdir.internal.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/conv.h"
#include "libc/sock/in.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/ifreq.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "third_party/musl/lookup.internal.h"
#include "third_party/musl/netdb.h"
#include "third_party/musl/resolv.internal.h"

asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2014 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");

#define PTR_MAX (64 + sizeof ".in-addr.arpa")
#define RR_PTR 12

static char *itoa(char *p, unsigned x) {
	p += 3*sizeof(int);
	*--p = 0;
	do {
		*--p = '0' + x % 10;
		x /= 10;
	} while (x);
	return p;
}

static void mkptr4(char *s, const unsigned char *ip)
{
	sprintf(s, "%d.%d.%d.%d.in-addr.arpa",
		ip[3], ip[2], ip[1], ip[0]);
}

static void mkptr6(char *s, const unsigned char *ip)
{
	static const char xdigits[] = "0123456789abcdef";
	int i;
	for (i=15; i>=0; i--) {
		*s++ = xdigits[ip[i]&15]; *s++ = '.';
		*s++ = xdigits[ip[i]>>4]; *s++ = '.';
	}
	strcpy(s, "ip6.arpa");
}

static void reverse_hosts(char *buf, const unsigned char *a, unsigned scopeid, int family)
{
	/* get path of /etc/hosts */
	char etc_hosts_buf[256];
	const char *etc_hosts_path =
		GetHostsTxtPath(etc_hosts_buf,
				sizeof(etc_hosts_buf));

	/* open hosts definitions file */
	unsigned char atmp[16];
	FILE *f = fopen(etc_hosts_path, "rbe");
	if (!f) return;
	if (family == AF_INET) {
		memcpy(atmp+12, a, 4);
		memcpy(atmp, "\0\0\0\0\0\0\0\0\0\0\xff\xff", 12);
		a = atmp;
	}

	/* read hosts definitions file */
	char line[512], *p, *z;
	struct address iplit;
	while (fgets(line, sizeof line, f)) {
		if ((p=strchr(line, '#'))) *p++='\n', *p=0;

		for (p=line; *p && !isspace(*p); p++);
		if (!*p) continue;
		*p++ = 0;
		if (__lookup_ipliteral(&iplit, line, AF_UNSPEC)<=0)
			continue;

		if (iplit.family == AF_INET) {
			memcpy(iplit.addr+12, iplit.addr, 4);
			memcpy(iplit.addr, "\0\0\0\0\0\0\0\0\0\0\xff\xff", 12);
			iplit.scopeid = 0;
		}

		if (memcmp(a, iplit.addr, 16) || iplit.scopeid != scopeid)
			continue;

		for (; *p && isspace(*p); p++);
		for (z=p; *z && !isspace(*z); z++);
		*z = 0;
		if (z-p < 256) {
			memcpy(buf, p, z-p+1);
			break;
		}
	}
	fclose(f);
}

static void reverse_services(char *buf, int port, int dgram)
{
	/* get path of /etc/services */
	char etc_services_buf[256];
	const char *etc_services_path =
		GetServicesTxtPath(etc_services_buf,
				   sizeof(etc_services_buf));

	/* open services definitions file */
	FILE *f = fopen(etc_services_path, "rbe");
	if (!f) return;

	/* read services definitions file */
	unsigned long svport;
	char line[128], *p, *z;
	while (fgets(line, sizeof line, f)) {
		if ((p=strchr(line, '#'))) *p++='\n', *p=0;

		for (p=line; *p && !isspace(*p); p++);
		if (!*p) continue;
		*p++ = 0;
		svport = strtoul(p, &z, 10);

		if (svport != port || z==p) continue;
		if (dgram && strncmp(z, "/udp", 4)) continue;
		if (!dgram && strncmp(z, "/tcp", 4)) continue;
		if (p-line > 32) continue;

		memcpy(buf, line, p-line);
		break;
	}
	fclose(f);
}

static int dns_parse_callback(void *c, int rr, const void *data, int len, const void *packet, int plen)
{
	if (rr != RR_PTR) return 0;
	if (__dn_expand(packet, (const unsigned char *)packet + plen,
	    data, c, 256) <= 0)
		*(char *)c = 0;
	return 0;

}

int getnameinfo(const struct sockaddr *restrict sa, socklen_t sl,
	char *restrict node, socklen_t nodelen,
	char *restrict serv, socklen_t servlen,
	int flags)
{
	char ptr[PTR_MAX];
	char buf[256], num[3*sizeof(int)+1];
	int af = sa->sa_family;
	unsigned char *a;
	unsigned scopeid;

	switch (af) {
	case AF_INET:
		a = (void *)&((struct sockaddr_in *)sa)->sin_addr;
		if (sl < sizeof(struct sockaddr_in)) return EAI_FAMILY;
		mkptr4(ptr, a);
		scopeid = 0;
		break;
	case AF_INET6:
		a = (void *)&((struct sockaddr_in6 *)sa)->sin6_addr;
		if (sl < sizeof(struct sockaddr_in6)) return EAI_FAMILY;
		if (memcmp(a, "\0\0\0\0\0\0\0\0\0\0\xff\xff", 12))
			mkptr6(ptr, a);
		else
			mkptr4(ptr, a+12);
		scopeid = ((struct sockaddr_in6 *)sa)->sin6_scope_id;
		break;
	default:
		return EAI_FAMILY;
	}

	if (node && nodelen) {
		buf[0] = 0;
		if (!(flags & NI_NUMERICHOST)) {
			reverse_hosts(buf, a, scopeid, af);
		}
		if (!*buf && !(flags & NI_NUMERICHOST)) {
			unsigned char query[18+PTR_MAX], reply[512];
			int qlen = __res_mkquery(0, ptr, 1, RR_PTR,
				0, 0, 0, query, sizeof query);
			query[3] = 0; /* don't need AD flag */
			int rlen = __res_send(query, qlen, reply, sizeof reply);
			buf[0] = 0;
			if (rlen > 0)
				__dns_parse(reply, rlen, dns_parse_callback, buf);
		}
		if (!*buf) {
			if (flags & NI_NAMEREQD) return EAI_NONAME;
			inet_ntop(af, a, buf, sizeof buf);
			if (scopeid) {
#if 1
                                /* TODO(jart): How do we handle this? */
                                return EAI_NONAME;
#else
				char *p = 0, tmp[IF_NAMESIZE+1];
				if (!(flags & NI_NUMERICSCOPE) &&
				    (IN6_IS_ADDR_LINKLOCAL(a) ||
				     IN6_IS_ADDR_MC_LINKLOCAL(a)))
					p = if_indextoname(scopeid, tmp+1);
				if (!p)
					p = itoa(num, scopeid);
				*--p = '%';
				strcat(buf, p);
#endif
			}
		}
		if (strlen(buf) >= nodelen) return EAI_OVERFLOW;
		strcpy(node, buf);
	}

	if (serv && servlen) {
		char *p = buf;
		int port = ntohs(((struct sockaddr_in *)sa)->sin_port);
		buf[0] = 0;
		if (!(flags & NI_NUMERICSERV))
			reverse_services(buf, port, flags & NI_DGRAM);
		if (!*p)
			p = itoa(num, port);
		if (strlen(p) >= servlen)
			return EAI_OVERFLOW;
		strcpy(serv, p);
	}

	return 0;
}
