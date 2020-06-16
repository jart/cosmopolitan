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
#include "libc/macros.h"
.source	__FILE__

/	Magic words to unbreak build if GCOV flags are passed.

__gcov_init:	
	ret
	.endfn	__gcov_init,globl,weak

__gcov_exit:
	ret
	.endfn	__gcov_exit,globl,weak

__gcov_merge_add:	
	ret
	.endfn	__gcov_merge_add,globl,weak

__gcov_fork:
	ret
	.endfn	__gcov_fork,globl,weak

__gcov_execle:
	ret
	.endfn	__gcov_execle,globl,weak

__gcov_execlp:
	ret
	.endfn	__gcov_execlp,globl,weak

__gcov_execl:
	ret
	.endfn	__gcov_execl,globl,weak

__gcov_execv:
	ret
	.endfn	__gcov_execv,globl,weak

__gcov_execvp:
	ret
	.endfn	__gcov_execvp,globl,weak

__gcov_execve:
	ret
	.endfn	__gcov_execve,globl,weak
