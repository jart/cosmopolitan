.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_SetWindowLongW,SetWindowLongW,2398

	.text.windows
SetWindowLong:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetWindowLongW(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetWindowLong,globl
	.previous
