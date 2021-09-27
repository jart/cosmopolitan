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
#include "libc/macros.internal.h"

// wontreturn void thread_spawn(int(*func)(void*), void* arg);
//
// This function is the root function of a new thread.
// It calls `func` with `arg`.
// The return value of `func` is passed as argument to the `exit` syscall.
//
// All arguments are passed onto the (newly created) stack.
// The stack must have been set as followed:
//              Top
//         +------------+
//         |    func    |
//         +------------+
//         |    arg     |
// %rsp -> +------------+
//

thread_spawn:
	.cfi_startproc
	/* stop stack trace from going deeper than this function */
	.cfi_undefined rip
	xor %rbp,%rbp
	/* pop arguments */
	pop %rdi
	pop %rax
	/* call function */
	call *%rax
	/* thread exit */
	mov %rax, %rdi
	mov 60, %rax
	syscall
	.cfi_endproc
	.endfn	thread_spawn,globl
	.source	__FILE__