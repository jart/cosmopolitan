.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_GetClientRect,GetClientRect,1815

	.text.windows
GetClientRect:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetClientRect(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetClientRect,globl
	.previous
