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
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/nt/enum/keyaccess.h"
#include "libc/nt/enum/reggetvalueflags.h"
#include "libc/nt/registry.h"
#include "libc/nt/runtime.h"
#include "libc/stdio/internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sock/sock.h"
#include "third_party/musl/lookup.internal.h"

asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2014 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");

// TODO(jart): ipv6 nameservers on windows
// TODO(jart): can we polyfill `search` on windows
static dontinline textwindows int __get_resolv_conf_nt(struct resolvconf *conf)
{
	int64_t hkInterfaces;
	uint32_t i, keycount;

	if (RegOpenKeyEx(
		    kNtHkeyLocalMachine,
		    u"SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces",
		    0, kNtKeyRead, &hkInterfaces)) {
		return __winerr();
	}

	if (RegQueryInfoKey(hkInterfaces, 0, 0, 0, &keycount, 0, 0, 0, 0, 0, 0, 0)) {
		RegCloseKey(hkInterfaces);
		return __winerr();
	}

	conf->nns = 0;
	for (i = 0; i < keycount; ++i) {
		char value8[128];
		uint32_t valuebytes, uuidlen;
		char16_t value[128], uuid[64];
		uuidlen = sizeof(uuid);
		if (!RegEnumKeyEx(hkInterfaces, i, uuid, &uuidlen, 0, 0, 0, 0) &&
		    ((!RegGetValue(hkInterfaces, uuid, u"DhcpIpAddress",
				   kNtRrfRtRegSz | kNtRrfRtRegMultiSz, NULL, value,
				   ((valuebytes = sizeof(value)), &valuebytes)) &&
		      valuebytes > 2 * sizeof(char16_t)) ||
		     (!RegGetValue(hkInterfaces, uuid, u"IpAddress",
				   kNtRrfRtRegSz | kNtRrfRtRegMultiSz, NULL, value,
				   ((valuebytes = sizeof(value)), &valuebytes)) &&
		      valuebytes > 2 * sizeof(char16_t))) &&
		    ((!RegGetValue(hkInterfaces, uuid, u"DhcpNameServer",
				   kNtRrfRtRegSz | kNtRrfRtRegMultiSz, NULL, value,
				   ((valuebytes = sizeof(value)), &valuebytes)) &&
		      valuebytes > 2 * sizeof(char16_t)) ||
		     (!RegGetValue(hkInterfaces, uuid, u"NameServer",
				   kNtRrfRtRegSz | kNtRrfRtRegMultiSz, NULL, value,
				   ((valuebytes = sizeof(value)), &valuebytes)) &&
		      valuebytes > 2 * sizeof(char16_t)))) {
			char *state, *addr, *tmp;
			tprecode16to8(value8, sizeof(value8), value);
			tmp = value8;
			while ((addr = strtok_r(tmp, ", ", &state))) {
				uint32_t ip;
				if (inet_pton(AF_INET, addr, &ip) == 1) {
					if (conf->nns < MAXNS) {
						conf->ns[conf->nns].family = AF_INET;
						conf->ns[conf->nns].scopeid = 0;
						memcpy(conf->ns[conf->nns].addr, &ip, 4);
						++conf->nns;
					}
				}
				tmp = NULL;
			}
		}
	}

	RegCloseKey(hkInterfaces);
	return 0;
}

int __get_resolv_conf(struct resolvconf *conf, char *search, size_t search_sz)
{
	FILE *f;
	int nns = 0;
	char line[256];

	conf->ndots = 1;
	conf->timeout = 5;
	conf->attempts = 2;
	if (search) *search = 0;

	f = fopen("/etc/resolv.conf", "rbe");
	if (!f) {
		if (errno == ENOENT ||
		    errno == ENOTDIR ||
		    errno == EACCES) {
			if (IsWindows())
				return __get_resolv_conf_nt(conf);
			goto no_resolv_conf;
		} else {
			return -1;
		}
        }

	while (fgets(line, sizeof line, f)) {
		char *p, *z;
		if (!strchr(line, '\n') && !feof(f)) {
			/* Ignore lines that get truncated rather than
			 * potentially misinterpreting them. */
			int c;
			do c = getc(f);
			while (c != '\n' && c != EOF);
			continue;
		}
		if (!strncmp(line, "options", 7) && isspace(line[7])) {
			p = strstr(line, "ndots:");
			if (p && isdigit(p[6])) {
				p += 6;
				unsigned long x = strtoul(p, &z, 10);
				if (z != p) conf->ndots = x > 15 ? 15 : x;
			}
			p = strstr(line, "attempts:");
			if (p && isdigit(p[9])) {
				p += 9;
				unsigned long x = strtoul(p, &z, 10);
				if (z != p) conf->attempts = x > 10 ? 10 : x;
			}
			p = strstr(line, "timeout:");
			if (p && (isdigit(p[8]) || p[8]=='.')) {
				p += 8;
				unsigned long x = strtoul(p, &z, 10);
				if (z != p) conf->timeout = x > 60 ? 60 : x;
			}
			continue;
		}
		if (!strncmp(line, "nameserver", 10) && isspace(line[10])) {
			if (nns >= MAXNS) continue;
			for (p=line+11; isspace(*p); p++);
			for (z=p; *z && !isspace(*z); z++);
			*z=0;
			if (__lookup_ipliteral(conf->ns+nns, p, AF_UNSPEC) > 0)
				nns++;
			continue;
		}

		if (!search) continue;
		if ((strncmp(line, "domain", 6) && strncmp(line, "search", 6))
		    || !isspace(line[6]))
			continue;
		for (p=line+7; isspace(*p); p++);
		size_t l = strlen(p);
		/* This can never happen anyway with chosen buffer sizes. */
		if (l >= search_sz) continue;
		memcpy(search, p, l+1);
	}

	fclose(f);

no_resolv_conf:
	if (!nns) {
		__lookup_ipliteral(conf->ns, "127.0.0.1", AF_UNSPEC);
		nns = 1;
	}

	conf->nns = nns;

	return 0;
}
