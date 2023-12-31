/*
 * Copyright (c) 1989 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Tony Nardo of the Johns Hopkins University/Applied Physics Lab.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/sock/sock.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/sock.h"
#include "libc/sock/struct/sockaddr.h"
#include "third_party/musl/netdb.h"
#include "third_party/finger/finger.h"

#ifndef lint
/*static char sccsid[] = "from: @(#)net.c	5.5 (Berkeley) 6/1/90";*/
char net_rcsid[] = "$Id: net.c,v 1.9 1999/09/14 10:51:11 dholland Exp $";
#endif /* not lint */

void netfinger(const char *name) {
	register FILE *fp;
	struct in_addr defaddr;
	register int c, sawret, ateol;
	struct hostent *hp, def;
	struct servent *sp;
	struct sockaddr_in sn;
	int s;
	char *alist[1], *host;

	host = strrchr(name, '@');
	if (!host) return;
	*host++ = '\0';

	memset(&sn, 0, sizeof(sn));

	sp = getservbyname("finger", "tcp");
	if (!sp) {
		eprintf("finger: tcp/finger: unknown service\n");
		return;
	}
	sn.sin_port = sp->s_port;

	hp = gethostbyname(host);
	if (!hp) {
		if (!inet_aton(host, &defaddr)) {
			eprintf("finger: unknown host: %s\n", host);
			return;
		}
		def.h_name = host;
		def.h_addr_list = alist;
		def.h_addr = (char *)&defaddr;
		def.h_length = sizeof(struct in_addr);
		def.h_addrtype = AF_INET;
		def.h_aliases = 0;
		hp = &def;
	}
	sn.sin_family = hp->h_addrtype;
	if (hp->h_length > (int)sizeof(sn.sin_addr)) {
	    hp->h_length = sizeof(sn.sin_addr);
	}
	memcpy(&sn.sin_addr, hp->h_addr, hp->h_length);

	if ((s = socket(hp->h_addrtype, SOCK_STREAM, 0)) < 0) {
		eprintf("finger: socket: %s\n", strerror(errno));
		return;
	}

	/* print hostname before connecting, in case it takes a while */
	xprintf("[%s]\n", hp->h_name);
	if (connect(s, (struct sockaddr *)&sn, sizeof(sn)) < 0) {
		eprintf("finger: connect: %s\n", strerror(errno));
		close(s);
		return;
	}

	/* -l flag for remote fingerd  */
	if (lflag) write(s, "/W ", 3);

	/* send the name followed by <CR><LF> */
	write(s, name, strlen(name));
	write(s, "\r\n", 2);

	/*
	 * Read from the remote system; once we're connected, we assume some
	 * data.  If none arrives, we hang until the user interrupts.
	 *
	 * If we see a <CR> or a <CR> with the high bit set, treat it as
	 * a newline; if followed by a newline character, only output one
	 * newline.
	 *
	 * Text is sent to xputc() for printability analysis.
	 */
	fp = fdopen(s, "r");
	if (!fp) {
		eprintf("finger: fdopen: %s\n", strerror(errno));
		close(s);
		return;
	}

	sawret = 0;
	ateol = 1;
	while ((c = getc(fp)) != EOF) {
		c &= 0xff;
		if (c == ('\r'|0x80) || c == ('\n'|0x80)) c &= 0x7f;
		if (c == '\r') {
			sawret = ateol = 1;
			xputc('\n');
		}
		else if (sawret && c == '\n') {
			sawret = 0;
			/* don't print */
		}
		else {
			if (c == '\n') ateol = 1;
			sawret = 0;
			xputc(c);
		}
	}
	if (!ateol) xputc('\n');
	fclose(fp);
}
