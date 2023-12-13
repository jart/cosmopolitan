/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│ vi: set noet ft=asm ts=8 sw=8 fenc=utf-8                                 :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/dce.h"
#include "libc/macros.internal.h"

//	Sections for varint encoded magic numbers.
//
//	These sections are all ordered by (group_name, constant_name).
//	They're populated by modules simply referencing the symbols.
//
//	@see libc/sysv/consts.sh
//	@see libc/sysv/consts/syscon_h
	.section .piro.bss.sort.syscon.1,"aw",@nobits
	.balign	8
syscon_start:/*
	...decentralized quadwords...
	*/.previous
	.section .piro.bss.sort.syscon.3,"aw",@nobits
syscon_end:
	.previous
	.type	syscon_start,@object
	.type	syscon_end,@object
	.globl	syscon_start
	.globl	syscon_end
#if SupportsLinux() || SupportsMetal()
	.section .sort.rodata.syscon.linux.1,"a",@progbits
syscon_linux:/*
	...decentralized leb128...
	*/.previous
	.type	syscon_linux,@object
	.globl	syscon_linux
#endif
#if SupportsXnu()
	.section .sort.rodata.syscon.xnu.1,"a",@progbits
syscon_xnu:/*
	...decentralized leb128...
	*/.previous
	.type	syscon_xnu,@object
	.globl	syscon_xnu
#endif
#if SupportsFreebsd()
	.section .sort.rodata.syscon.freebsd.1,"a",@progbits
syscon_freebsd:/*
	...decentralized leb128...
	*/.previous
	.type	syscon_freebsd,@object
	.globl	syscon_freebsd
#endif
#if SupportsOpenbsd()
	.section .sort.rodata.syscon.openbsd.1,"a",@progbits
syscon_openbsd:/*
	...decentralized leb128...
	*/.previous
	.type	syscon_openbsd,@object
	.globl	syscon_openbsd
#endif
#if SupportsNetbsd()
	.section .sort.rodata.syscon.netbsd.1,"a",@progbits
syscon_netbsd:/*
	...decentralized leb128...
	*/.previous
	.type	syscon_netbsd,@object
	.globl	syscon_netbsd
#endif
#if SupportsWindows()
	.section .sort.rodata.syscon.windows.1,"a",@progbits
syscon_windows:/*
	...decentralized leb128...
	*/.previous
	.type	syscon_windows,@object
	.globl	syscon_windows
#endif
