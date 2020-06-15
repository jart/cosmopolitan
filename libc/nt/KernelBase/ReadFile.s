.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_ReadFile,ReadFile,1311

	.text.windows
ReadFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_ReadFile(%rip),%rax
	jmp	__sysv2nt6
	.endfn	ReadFile,globl
	.previous
