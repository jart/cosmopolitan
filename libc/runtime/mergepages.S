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
#include "libc/sysv/consts/madv.h"
#include "libc/sysv/consts/nr.h"
#include "libc/macros.h"
.text.startup
.source	__FILE__

/	Merges page table entries for pages with identical content.
/
/	This is a hint. It can help trim physical memory for things
/	like extremely sparse data.
/
/	@param	rdi is base address
/	@param	rsi is byte length
mergepages:
	.leafprologue
	.profilable
	mov	$-PAGESIZE,%rax
	and	%rax,%rdi
	and	%rax,%rsi
	mov	__NR_madvise,%eax
	mov	MADV_MERGEABLE,%edx
	test	%edx,%edx
	jz	1f
	syscall
1:	.leafepilogue
	.endfn	mergepages,globl
