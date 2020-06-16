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
#include "libc/dce.h"
#include "libc/macros.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/notice.inc"

/	Computes 32-bit Castagnoli Cyclic Redundancy Check.
/
/	@param	edi is the initial hash value (0 is fine)
/	@param	rsi points to the data
/	@param	rdx is the byte size of data
/	@return	eax is the new hash value
/	@note	Used by ISCSI, TensorFlow, etc.
	.initbss 300,_init_crc32c
crc32c:	.quad	0
	.endobj	crc32c,globl
	.previous

	.init.start 300,_init_crc32c
	ezlea	crc32c$pure,ax
#if !IsTiny()
	ezlea	crc32c$sse42,cx
	testb	X86_HAVE(SSE4_2)+kCpuids(%rip)
	cmovnz	%rcx,%rax
#endif
	stosq
	.init.end 300,_init_crc32c
	.source	__FILE__
