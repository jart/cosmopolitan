.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_InsertMenuA,InsertMenuA,2041

	.text.windows
InsertMenuA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_InsertMenuA(%rip),%rax
	jmp	__sysv2nt6
	.endfn	InsertMenuA,globl
	.previous
