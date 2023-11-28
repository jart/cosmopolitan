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
#include "libc/calls/struct/termios.h"
#include "libc/calls/struct/winsize.h"
#include "libc/calls/termios.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/termios.h"
#include "third_party/finger/finger.h"

int
getscreenwidth(void)
{
	struct winsize ws;
	if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) < 0 || ws.ws_col==0) {
		return 80;
	}
	return ws.ws_col;
}

int
is8bit(void)
{
	static int cache=-1;
	struct termios tios;
	if (cache>=0) return cache;

	if (tcgetattr(STDIN_FILENO, &tios)<0) {
		/* assume 8-bit; it's 1999 now, not 1972 */
		cache = 1;
	}
	else {
		cache = (tios.c_cflag & CSIZE)==CS8;
	}
	return cache;
}

/************/

static int send_crs=0;

void
set_crmode(void)
{
	send_crs = 1;
}

static
void
fxputc(FILE *f, int ch)
{
	/* drop any sign */
	ch = ch&0xff;

	/* on 7-bit terminals, strip high bit */
	if (!is8bit()) ch &= 0x7f; 

	/* 
	 * Assume anything that isn't a control character is printable.
	 * We can't count on locale stuff to tell us what's printable 
	 * because we might be looking at someone who uses different
	 * locale settings or is on the other side of the planet. So,
	 * strip 0-31, 127, 128-159, and 255. Note that not stripping
	 * 128-159 is asking for trouble, as 155 (M-esc) is interpreted
	 * as esc-[ by most terminals. Hopefully this won't break anyone's
	 * charset.
	 * 
	 * It would be nice if we could set the terminal to display in the
	 * right charset, but we have no way to know what it is. feh.
	 */
	
	if (((ch&0x7f) >= 32 && (ch&0x7f) != 0x7f) || ch=='\t') {
		putc(ch, f);
		return;
	}

	if (ch=='\n') {
		if (send_crs) putc('\r', f);
		putc('\n', f);
		return;
	}

	if (ch&0x80) {
		putc('M', f);
		putc('-', f);
		ch &= 0x7f;
	}

	putc('^', f);
	if (ch==0x7f) putc('?', f);
	else putc(ch+'@', f);
}

void
xputc(int ch)
{
	fxputc(stdout, ch);
}

static void fxputs(FILE *f, const char *buf) {
	int i;
	for (i=0; buf[i]; i++) fxputc(f, buf[i]);
}

int xprintf(const char *fmt, ...) {
	char buf[1024];
	va_list ap;
	
	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	
	fxputs(stdout, buf);
	
	return strlen(buf);
}

int eprintf(const char *fmt, ...) {
	char buf[1024];
	va_list ap;
	
	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);

	fxputs(stderr, buf);
	
	return strlen(buf);
}
