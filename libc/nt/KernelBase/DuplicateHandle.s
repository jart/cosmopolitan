.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_DuplicateHandle,DuplicateHandle,283

	.text.windows
DuplicateHandle:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_DuplicateHandle(%rip),%rax
	jmp	__sysv2nt8
	.endfn	DuplicateHandle,globl
	.previous
