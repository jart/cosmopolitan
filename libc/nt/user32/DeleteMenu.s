.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_DeleteMenu,DeleteMenu,1681

	.text.windows
DeleteMenu:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_DeleteMenu(%rip),%rax
	jmp	__sysv2nt
	.endfn	DeleteMenu,globl
	.previous
