.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_SetTimer,SetTimer,2384

	.text.windows
SetTimer:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetTimer(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetTimer,globl
	.previous
