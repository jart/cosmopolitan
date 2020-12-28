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
/* clang-format off */
.include "libc/macros.internal.inc"

.macro	.syscon	group:req name:req linux:req xnu:req freebsd:req openbsd:req windows:req
	yoink	_init_systemfive
 .section .piro.bss.sort.syscon.2.\group\().\name,"aw",@nobits
\name:	.quad	0
	.endobj	\name,globl,hidden
 .previous
 .section .sort.rodata.syscon.linux.2.\group\().\name,"a",@progbits
	.sleb128 \linux
 .previous
 .section .sort.rodata.syscon.xnu.2.\group\().\name,"a",@progbits
	.sleb128 \xnu
 .previous
 .section .sort.rodata.syscon.freebsd.2.\group\().\name,"a",@progbits
	.sleb128 \freebsd
 .previous
 .section .sort.rodata.syscon.openbsd.2.\group\().\name,"a",@progbits
	.sleb128 \openbsd
 .previous
 .section .sort.rodata.syscon.windows.2.\group\().\name,"a",@progbits
	.sleb128 \windows
 .previous
.endm
