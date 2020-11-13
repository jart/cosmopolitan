.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_FillRect,FillRect,1780

	.text.windows
FillRect:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_FillRect(%rip),%rax
	jmp	__sysv2nt
	.endfn	FillRect,globl
	.previous
