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
#include "libc/dce.h"
#include "libc/macros.internal.h"
.privileged

//	Performs raw System Five system call.
//
//	This function provides a direct path into system call support
//	that's friendly to C code, since it doesn't need an intermediate
//	thunk. It only supports arities up to six, since there's no way
//	to do more safely; this isn't a problem with Linux, although
//	certain BSD calls may not be available.
//
//	@param	%rdi is system call ordinal, which isn't translated,
//		and must be correct for the underlying host system
//	@param	%rsi,%rdx,%rcx,%r8,%r9 may supply parameters 1 through 5
//	@param	sixth is optionally pushed on the stack before call
//	@return	%rax has result, or -1 w/ errno on failure
syscall:
	push	%rbp
	mov	%rsp,%rbp
	.profilable

//	slide arguments into their right places
	mov	%rdi,%rax		# nr
	mov	%rsi,%rdi		# arg 1
	mov	%rdx,%rsi		# arg 2
	mov	%rcx,%rdx		# arg 3
	mov	%r8,%rcx		# arg 4
	mov	%r9,%r8			# arg 5
	mov	16(%rbp),%r9		# arg 6
	push	32(%rbp)		# arg 8
	push	24(%rbp)		# arg 7

//	convert from consts.sh to syscalls.sh encoding
	push	%rcx
	mov	__hostos(%rip),%cl
	test	$_HOSTLINUX,%cl
	jnz	2f
1:	test	$_HOSTFREEBSD,%cl
	jz	1f
	shl	$4*7,%rax
	jmp	2f
1:	test	$_HOSTOPENBSD,%cl
	jz	1f
	shl	$4*10,%rax
	jmp	2f
1:	test	$_HOSTNETBSD,%cl
	jz	1f
	shl	$4*13,%rax
	jmp	2f
1:	test	$_HOSTXNU,%cl
	jz	2f
	mov	%eax,%ecx
	and	$0x0f000000,%ecx
	and	$0x00000fff,%eax
	shl	$4*3,%eax
	or	%ecx,%eax
2:	pop	%rcx

//	trigger the system call
	call	*__systemfive(%rip)

//	clean up stack and return
	leave
	ret
	.endfn	syscall,globl
