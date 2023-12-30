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
#include "libc/str/str.h"
#include "third_party/musl/netdb.h"

asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2014 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");

/* do we really need all these?? */

static int idx;
static const unsigned char protos[] = {
	"\000ip\0"
	"\001icmp\0"
	"\002igmp\0"
	"\003ggp\0"
	"\004ipencap\0"
	"\005st\0"
	"\006tcp\0"
	"\010egp\0"
	"\014pup\0"
	"\021udp\0"
	"\024hmp\0"
	"\026xns-idp\0"
	"\033rdp\0"
	"\035iso-tp4\0"
	"\044xtp\0"
	"\045ddp\0"
	"\046idpr-cmtp\0"
	"\051ipv6\0"
	"\053ipv6-route\0"
	"\054ipv6-frag\0"
	"\055idrp\0"
	"\056rsvp\0"
	"\057gre\0"
	"\062esp\0"
	"\063ah\0"
	"\071skip\0"
	"\072ipv6-icmp\0"
	"\073ipv6-nonxt\0"
	"\074ipv6-opts\0"
	"\111rspf\0"
	"\121vmtp\0"
	"\131ospf\0"
	"\136ipip\0"
	"\142encap\0"
	"\147pim\0"
	"\377raw"
};

void endprotoent(void)
{
	idx = 0;
}

void setprotoent(int stayopen)
{
	idx = 0;
}

struct protoent *getprotoent(void)
{
	static struct protoent p;
	static const char *aliases;
	if (idx >= sizeof protos) return NULL;
	p.p_proto = protos[idx];
	p.p_name = (char *)&protos[idx+1];
	p.p_aliases = (char **)&aliases;
	idx += strlen(p.p_name) + 2;
	return &p;
}

struct protoent *getprotobyname(const char *name)
{
	struct protoent *p;
	endprotoent();
	do p = getprotoent();
	while (p && strcmp(name, p->p_name));
	return p;
}

struct protoent *getprotobynumber(int num)
{
	struct protoent *p;
	endprotoent();
	do p = getprotoent();
	while (p && p->p_proto != num);
	return p;
}
