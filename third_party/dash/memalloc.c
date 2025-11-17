/*-
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 * Copyright (c) 1997-2005
 *	Herbert Xu <herbert@gondor.apana.org.au>.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Kenneth Almquist.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
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

#include <stdlib.h>
#include <unistd.h>

#include "shell.h"
#include "output.h"
#include "memalloc.h"
#include "error.h"
#include "machdep.h"
#include "mystring.h"
#include "system.h"

static __attribute__((__always_inline__)) inline void outofspace(void)
{
	sh_error("Out of space");
}

static void *checknull(void *p)
{
	if (!p)
		outofspace();
	return p;
}

/*
 * Like malloc, but returns an error when out of space.
 */

__attribute__((__noinline__)) void *ckmalloc(size_t nbytes)
{
	void *p;

	p = malloc(nbytes);
	return checknull(p);
}


/*
 * Same for realloc.
 */

__attribute__((__noinline__)) void *ckrealloc(void *p, size_t nbytes)
{
	p = realloc(p, nbytes);
	return checknull(p);
}


/*
 * Make a copy of a string in safe storage.
 */

char *
savestr(const char *s)
{
	return checknull(strdup(s));
}


/*
 * Parse trees for commands are allocated in lifo order, so we use a stack
 * to make this more efficient, and also to avoid all sorts of exception
 * handling code to handle interrupts in the middle of a parse.
 *
 * The size 504 was chosen because the Ultrix malloc handles that size
 * well.
 */

/* minimum size of a block */
#define MINSIZE SHELL_ALIGN(504)

struct stack_block {
	struct stack_block *prev;
	char space[MINSIZE];
};

struct stack_block stackbase;
struct stack_block *stackp = &stackbase;
char *stacknxt = stackbase.space;
size_t stacknleft = MINSIZE;
char *sstrend = stackbase.space + MINSIZE;

void *stalloc(size_t nbytes)
{
	char *p;
	size_t aligned;

	aligned = SHELL_ALIGN(nbytes);
	if (aligned > stacknleft) {
		size_t len;
		size_t blocksize;
		struct stack_block *sp;

		blocksize = aligned;
		if (blocksize < MINSIZE)
			blocksize = MINSIZE;
		len = sizeof(struct stack_block) - MINSIZE + blocksize;
		if (len < blocksize)
			outofspace();
		INTOFF;
		sp = ckmalloc(len);
		sp->prev = stackp;
		stacknxt = sp->space;
		stacknleft = blocksize;
		sstrend = stacknxt + blocksize;
		stackp = sp;
		INTON;
	}
	p = stacknxt;
	stacknxt += aligned;
	stacknleft -= aligned;
	return p;
}


void stunalloc(void *p)
{
#ifdef DEBUG
	if (!p || (stacknxt < (char *)p) || ((char *)p < stackp->space)) {
		write(2, "stunalloc\n", 10);
		abort();
	}
#endif
	stacknleft += stacknxt - (char *)p;
	stacknxt = p;
}



__attribute__((__noinline__)) void pushstackmark(struct stackmark *mark,
						 size_t len)
{
	mark->stackp = stackp;
	mark->stacknxt = stacknxt;
	mark->stacknleft = stacknleft;
	grabstackblock(len);
}

void setstackmark(struct stackmark *mark)
{
	pushstackmark(mark, stacknxt == stackp->space && stackp != &stackbase);
}


void
popstackmark(struct stackmark *mark)
{
	struct stack_block *sp;

	INTOFF;
	while (stackp != mark->stackp) {
		sp = stackp;
		stackp = sp->prev;
		ckfree(sp);
	}
	stacknxt = mark->stacknxt;
	stacknleft = mark->stacknleft;
	sstrend = mark->stacknxt + mark->stacknleft;
	INTON;
}


/*
 * When the parser reads in a string, it wants to stick the string on the
 * stack and only adjust the stack pointer when it knows how big the
 * string is.  Stackblock (defined in stack.h) returns a pointer to a block
 * of space on top of the stack and stackblocklen returns the length of
 * this block.  Growstackblock will grow this space by at least one byte,
 * possibly moving it (like realloc).  Grabstackblock actually allocates the
 * part of the block that has been used.
 */

static char *growstackblock(size_t min)
{
	size_t newlen;
	char *p;

	newlen = stacknleft * 2;
	if (newlen < stacknleft)
		outofspace();
	min = SHELL_ALIGN(min | 128);
	if (newlen < min)
		newlen += min;

	if (stacknxt == stackp->space && stackp != &stackbase) {
		struct stack_block *sp;
		struct stack_block *prevstackp;
		size_t grosslen;

		INTOFF;
		sp = stackp;
		prevstackp = sp->prev;
		grosslen = newlen + sizeof(struct stack_block) - MINSIZE;
		sp = ckrealloc(sp, grosslen);
		sp->prev = prevstackp;
		stackp = sp;
		p = stacknxt = sp->space;
		stacknleft = newlen;
		sstrend = sp->space + newlen;
		INTON;
	} else {
		char *oldspace = stacknxt;
		int oldlen = stacknleft;

		p = stalloc(newlen);

		/* free the space we just allocated */
		stacknxt = memcpy(p, oldspace, oldlen);
		stacknleft += newlen;
	}

	return p;
}

/*
 * The following routines are somewhat easier to use than the above.
 * The user declares a variable of type STACKSTR, which may be declared
 * to be a register.  The macro STARTSTACKSTR initializes things.  Then
 * the user uses the macro STPUTC to add characters to the string.  In
 * effect, STPUTC(c, p) is the same as *p++ = c except that the stack is
 * grown as necessary.  When the user is done, she can just leave the
 * string there and refer to it using stackblock().  Or she can allocate
 * the space for it using grabstackstr().  If it is necessary to allow
 * someone else to use the stack temporarily and then continue to grow
 * the string, the user should use grabstack to allocate the space, and
 * then call ungrabstr(p) to return to the previous mode of operation.
 *
 * USTPUTC is like STPUTC except that it doesn't check for overflow.
 * CHECKSTACKSPACE can be called before USTPUTC to ensure that there
 * is space for at least one character.
 */

void *
growstackstr(void)
{
	size_t len = stackblocksize();

	return growstackblock(0) + len;
}

__attribute__((__noinline__)) char *growstackto(size_t len)
{
	if (stackblocksize() < len)
		return growstackblock(len);
	return stackblock();
}

/*
 * Called from CHECKSTRSPACE.
 */

__attribute__((__noinline__)) char *makestrspace(size_t newlen, char *p)
{
	size_t len = p - stacknxt;

	return growstackto(len + newlen) + len;
}

__attribute__((__noinline__)) char *stnputs(const char *s, size_t n, char *p)
{
	p = makestrspace(n, p);
	p = mempcpy(p, s, n);
	return p;
}

char *
stputs(const char *s, char *p)
{
	return stnputs(s, strlen(s), p);
}
