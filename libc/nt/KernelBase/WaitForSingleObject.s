.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_WaitForSingleObject,WaitForSingleObject,1779

	.text.windows
WaitForSingleObject:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WaitForSingleObject(%rip),%rax
	jmp	__sysv2nt
	.endfn	WaitForSingleObject,globl
	.previous
