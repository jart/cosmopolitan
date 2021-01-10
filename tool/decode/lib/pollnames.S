/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "ape/relocations.h"
#include "ape/relocations.h"
#include "libc/macros.h"

	.Lrows = 0			# w/ 2 cols

	.macro	.tab	sym:req str
	.section .rodata.str1.1,"aSM",@progbits,1
.L\@:	.asciz	"\str"
	.previous
	.long	RVA(\sym)
	.long	RVA(.L\@)
	.Lrows = .Lrows + 1
	.endm

	.initro 301,_init_kPollNames
kPollNamesRo:
        .tab	POLLNVAL	"POLLNVAL"
	.tab	POLLWRNORM	"POLLWRNORM"
	.tab	POLLWRBAND	"POLLWRBAND"
	.tab	POLLRDNORM	"POLLRDNORM"
	.tab	POLLRDHUP	"POLLRDHUP"
	.tab	POLLRDBAND	"POLLRDBAND"
	.tab	POLLHUP		"POLLHUP"
	.tab	POLLERR		"POLLERR"
	.tab	POLLPRI		"POLLPRI"
	.tab	POLLOUT		"POLLOUT"
	.tab	POLLIN		"POLLIN"
	.endobj	kPollNamesRo,globl,hidden
	.previous

/	Mapping of poll() flags to their string names.
/	@see	RecreateFlags()
	.initbss 301,_init_kPollNames
kPollNames:
	.rept	.Lrows
	.quad	0			# unsigned long id
	.quad	0			# const char *const name
	.endr
	.quad	0,0			# terminator row
	.endobj	kPollNames,globl
	.previous

	.init.start 301,_init_kPollNames
	pushpop	.Lrows,%rcx		# relocate RO→BSS b/c -fPIE crap
0:	lodsl
	mov	(%rbx,%rax),%rax	# read what systemfive.S decoded
	stosq
	lodsl
	add	%rbx,%rax		# %rbx is image base (cosmo abi)
	stosq
	.loop	0b
	add	$16,%rdi
	.init.end 301,_init_kPollNames
