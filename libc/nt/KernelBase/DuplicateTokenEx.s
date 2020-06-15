.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_DuplicateTokenEx,DuplicateTokenEx,286

	.text.windows
DuplicateTokenEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_DuplicateTokenEx(%rip),%rax
	jmp	__sysv2nt6
	.endfn	DuplicateTokenEx,globl
	.previous
