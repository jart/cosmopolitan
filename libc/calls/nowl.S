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
#include "libc/nexgen32e/x86feature.h"
#include "libc/macros.h"

/	Returns timestamp without needing system calls.
/
/	@return	seconds since unix epoch in %st0
/	@note	uses microsecond scale fallback on k8 or vm
	.initbss 202,_init_nowl
nowl:	.quad	0
	.endobj	nowl,globl
	.previous

	.init.start 202,_init_nowl
	ezlea	nowl$sys,ax
	ezlea	nowl$art,cx
	testb	X86_HAVE(INVTSC)+kCpuids(%rip)
	cmovnz	%rcx,%rax
	stosq
	.init.end 202,_init_nowl
	.source	__FILE__
