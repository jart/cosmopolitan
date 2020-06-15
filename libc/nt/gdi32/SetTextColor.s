.include "o/libc/nt/codegen.inc"
.imp	gdi32,__imp_SetTextColor,SetTextColor,1912

	.text.windows
SetTextColor:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetTextColor(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetTextColor,globl
	.previous
