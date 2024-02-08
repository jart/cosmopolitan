/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│ vi: set noet ft=asm ts=8 sw=8 fenc=utf-8                                 :vi │
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
#include "libc/macros.internal.h"

//	Magic words to unbreak build if GCOV flags are passed.

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
