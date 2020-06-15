/*-*- mode:asm; indent-tabs-mode:t; tab-width:8; coding:utf-8               -*-│
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
	__piro_start = 0
	__relo_end = 0
	__piro_end = 0

	.globl	_base
	.globl	ape.xlm
	.globl	__piro_start
	.globl	__relo_end
	.globl	__piro_end
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
	.weak	__piro_start
	.weak	__relo_end
	.weak	__piro_end
	.weak	__privileged_start
	.weak	__ro
	.weak	__test_start
	.weak	_edata
	.weak	_ehead
	.weak	_end
	.weak	_ereal
	.weak	_etext
