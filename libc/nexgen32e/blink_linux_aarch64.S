/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
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
#include "libc/macros.internal.h"

//	Blink Virtual Machine for Linux Arm64
//
//	If you want to support Raspberry Pi by embedding an emulator
//	in your APE binary that runs automatically, then put this:
//
//	    __static_yoink("blink_linux_aarch64");
//
//	In your main.c file, to pull it into linkage from the static
//	archive. Alternatively, you may simply add blink_linux_aarch64.o
//	as an explicit linker argument.

	.section .blink,"a",@progbits
	.globl	blink_linux_aarch64_size
blink_linux_aarch64:
	.incbin	"ape/blink-linux-aarch64.gz"
	.endobj	blink_linux_aarch64,globl
blink_linux_aarch64_size = . - blink_linux_aarch64

        .section .emush,"a",@progbits
	.ascii	"if [ \"$s\" = Linux ]; then\n"
	.ascii	"if [ \"$m\" = aarch64 ] || [ \"$m\" = arm64 ]; then\n"
	.ascii	"if ! [ -x \"$e\" ]; then\n"
	.ascii	"echo \"extracting blink-linux-aarch64 to ${e}\" >&2\n"
	.ascii	"dd if=\"$o\" bs=1 skip=$(("
	.weak	blink_linux_aarch64_quad
	.quad	blink_linux_aarch64_quad
	.weak	blink_linux_aarch64_short
	.short	blink_linux_aarch64_short
	.ascii	")) count=$(("
	.weak	blink_linux_aarch64_size_quad
	.quad	blink_linux_aarch64_size_quad
	.weak	blink_linux_aarch64_size_short
	.short	blink_linux_aarch64_size_short
	.ascii	")) conv=notrunc 2>/dev/null | gunzip >\"$e.$$\"\n"
	.ascii	"mv -f \"$e.$$\" \"$e\"\n"
	.ascii	"chmod +x \"$e\"\n"
	.ascii	"fi\n"
	.ascii	"exec \"$e\" \"$o\" \"$@\"\n"
	.ascii	"fi\n"
	.ascii	"fi\n"
