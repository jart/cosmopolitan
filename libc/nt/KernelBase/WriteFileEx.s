.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_WriteFileEx,WriteFileEx,1824

	.text.windows
WriteFileEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WriteFileEx(%rip),%rax
	jmp	__sysv2nt6
	.endfn	WriteFileEx,globl
	.previous
