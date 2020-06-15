.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_SetParent,SetParent,2357

	.text.windows
SetParent:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetParent(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetParent,globl
	.previous
