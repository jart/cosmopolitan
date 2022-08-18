/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 sw=8 fenc=utf-8                                     :vi│
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
#include "libc/dce.h"
#include "libc/macros.internal.h"
/* clang-format off */

.macro	.syscon	group:req name:req linux:req xnu:req freebsd:req openbsd:req netbsd:req windows:req
	.yoink	_init_systemfive

	.section .piro.bss.sort.syscon.2.\group\().\name,"aw",@nobits
\name:	.quad	0
	.endobj	\name,globl
	.previous

#if SupportsLinux() || SupportsMetal()
	.section .sort.rodata.syscon.linux.2.\group\().\name,"a",@progbits
	.uleb128 \linux
	.previous
#endif

#if SupportsXnu()
	.section .sort.rodata.syscon.xnu.2.\group\().\name,"a",@progbits
	.uleb128 \xnu
	.previous
#endif

#if SupportsFreebsd()
	.section .sort.rodata.syscon.freebsd.2.\group\().\name,"a",@progbits
	.uleb128 \freebsd
	.previous
#endif

#if SupportsOpenbsd()
	.section .sort.rodata.syscon.openbsd.2.\group\().\name,"a",@progbits
	.uleb128 \openbsd
	.previous
#endif

#if SupportsNetbsd()
	.section .sort.rodata.syscon.netbsd.2.\group\().\name,"a",@progbits
	.uleb128 \netbsd
	.previous
#endif

#if SupportsWindows()
	.section .sort.rodata.syscon.windows.2.\group\().\name,"a",@progbits
	.uleb128 \windows
	.previous
#endif

.endm
