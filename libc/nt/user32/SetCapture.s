.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_SetCapture,SetCapture,2310

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
