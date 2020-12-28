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

/	Traditional executable boundaries defined by linker.
/	@see	man etext
	_etext = 0
	_edata = 0
	_end = 0

/	Cosmopolitan executable boundaries defined by linker script.
/	@see	libc/elf/elf.lds
/	@see	ape/ape.lds
	_base = 0
	ape.xlm = 0
	_ehead = 0
	_ereal = 0
	__privileged_start = 0
	__test_start = 0
	__ro = 0
	__relo_start = 0
	__relo_end = 0

	.globl	_base
	.globl	ape.xlm
	.globl	__relo_start
	.globl	__relo_end
	.globl	__privileged_start
	.globl	__ro
	.globl	__test_start
	.globl	_edata
	.globl	_ehead
	.globl	_end
	.globl	_ereal
	.globl	_etext

	.weak	_base
	.weak	ape.xlm
	.weak	__relo_start
	.weak	__relo_end
	.weak	__privileged_start
	.weak	__ro
	.weak	__test_start
	.weak	_edata
	.weak	_ehead
	.weak	_end
	.weak	_ereal
	.weak	_etext
