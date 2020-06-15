.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_WriteFile,WriteFile,1823

	.text.windows
WriteFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WriteFile(%rip),%rax
	jmp	__sysv2nt6
	.endfn	WriteFile,globl
	.previous
