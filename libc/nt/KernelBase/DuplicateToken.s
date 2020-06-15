.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_DuplicateToken,DuplicateToken,285

	.text.windows
DuplicateToken:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_DuplicateToken(%rip),%rax
	jmp	__sysv2nt
	.endfn	DuplicateToken,globl
	.previous
