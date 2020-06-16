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
#include "libc/macros.h"

/	Returns true if architecture guarantees atomicity.
/
/	@param	rdi is byte width of type
/	@param	rsi is optional pointer for alignment check
/	@see	Intel's Six Thousand Page Manual V.3A §8.2.3.1
__atomic_is_lock_free:
	.leafprologue
	.profilable
	xor	%ecx,%ecx
	pushpop	1,%rax
	cmp	$7,%rdi
	cmova	%ecx,%eax
	dec	%edi
	and	%edi,%esi
	cmovnz	%ecx,%eax
	.leafepilogue
	.endfn	__atomic_is_lock_free,globl,hidden
	.source	__FILE__
