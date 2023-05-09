#include "libc/nt/codegen.h"
.imp	user32,__imp_SetCapture,SetCapture,2310

	.text.windows
SetCapture:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_SetCapture(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	SetCapture,globl
	.previous
