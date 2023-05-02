#include "libc/nt/codegen.h"
.imp	user32,__imp_SetCapture,SetCapture,2310

#ifdef __x86_64__
	.text.windows
SetCapture:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_SetCapture(%rip)
	leave
	ret
	.endfn	SetCapture,globl
	.previous
#endif
