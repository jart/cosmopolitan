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

//	Traditional executable boundaries defined by linker.
//	@see	man etext
	_etext = 0
	_edata = 0
	_end = 0

//	Cosmopolitan executable boundaries defined by linker script.
//	@see	libc/elf/elf.lds
//	@see	ape/ape.lds
	_base = 0
	ape_xlm = 0
	_ehead = 0
	_ezip = 0
	_ereal = 0
	__privileged_start = 0
	__privileged_end = 0
	__privileged_addr = 0
	__privileged_size = 0
	__test_start = 0
	__ro = 0
	__relo_start = 0
	__relo_end = 0
	__data_start = 0
	__data_end = 0
	__bss_start = 0
	__bss_end = 0

//	Thread local boundaries defined by linker script
//	@see	ape/ape.lds
	_tdata_start = 0
	_tdata_end = 0
	_tdata_size = 0
	_tbss_start = 0
	_tbss_end = 0
	_tbss_offset = 0
	_tbss_size = 0
	_tls_size = 0
	_tls_content = 0

	.globl	_base
	.globl	ape_xlm
	.globl	__relo_start
	.globl	__relo_end
	.globl	__privileged_size
	.globl	__privileged_addr
	.globl	__privileged_start
	.globl	__privileged_end
	.globl	__ro
	.globl	__test_start
	.globl	_edata
	.globl	_ehead
	.globl	_end
	.globl	_ezip
	.globl	_ereal
	.globl	_etext
	.globl	_tdata_start
	.globl	_tdata_end
	.globl	_tdata_size
	.globl	_tbss_start
	.globl	_tbss_end
	.globl	_tbss_size
	.globl	_tbss_offset
	.globl	_tls_size
	.globl	_tls_content
	.globl	__data_start
	.globl	__data_end
	.globl	__bss_start
	.globl	__bss_end

	.weak	_base
	.weak	ape_xlm
	.weak	__relo_start
	.weak	__relo_end
	.weak	__privileged_size
	.weak	__privileged_addr
	.weak	__privileged_start
	.weak	__privileged_end
	.weak	__ro
	.weak	__test_start
	.weak	_edata
	.weak	_ehead
	.weak	_end
	.weak	_ezip
	.weak	_ereal
	.weak	_etext
	.weak	_tdata_start
	.weak	_tdata_end
	.weak	_tdata_size
	.weak	_tbss_start
	.weak	_tbss_end
	.weak	_tbss_size
	.weak	_tls_size
	.weak	_tls_content
	.weak	_tbss_offset
	.weak	__data_start
	.weak	__data_end
	.weak	__bss_start
	.weak	__bss_end
