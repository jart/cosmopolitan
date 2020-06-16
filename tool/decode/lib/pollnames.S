/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "ape/relocations.h"
#include "ape/relocations.h"
#include "libc/macros.h"

	.Lrows = 0			# w/ 2 cols

	.macro	.tab	sym:req str
	.pushsection .rodata.str1.1,"aSM",@progbits,1
.L\@:	.asciz	"\str"
	.popsection
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
/	@see	recreateflags()
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
	loop	0b
	add	$16,%rdi
	.init.end 301,_init_kPollNames
