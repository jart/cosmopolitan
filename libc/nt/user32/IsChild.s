.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_IsChild,IsChild,2059

	.text.windows
IsChild:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_IsChild(%rip),%rax
	jmp	__sysv2nt
	.endfn	IsChild,globl
	.previous
