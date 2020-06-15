.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_WriteFileGather,WriteFileGather,1825

	.text.windows
WriteFileGather:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WriteFileGather(%rip),%rax
	jmp	__sysv2nt6
	.endfn	WriteFileGather,globl
	.previous
