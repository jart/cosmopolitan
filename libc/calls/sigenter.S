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
#include "libc/calls/internal.h"
#include "libc/macros.h"
.source	__FILE__

/	System Five signal handler.
/
/	This is needed because (1) a signal is allowed to trigger at
/	just about any time, and leaf functions (e.g. memcpy) aren't
/	required to leave Cosmopolitan's image base register alone.
/
/	@param	%edi is the signal number
/	@param	%rsi will be passed for sigactions
/	@param	%rdx will be passed for sigactions
/	@return	true if handler was invoked
__sigenter:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	and	$NSIG-1,%edi
	mov	g_sighandrvas(,%rdi,4),%eax
	cmp	$kSigactionMinRva,%eax
	jl	2f
	lea	_base(%rax),%eax
	call	*%rax
	mov	$1,%eax
1:	leave
	ret
2:	xor	%eax,%eax
	jmp	1b
	.endfn	__sigenter,globl,hidden
