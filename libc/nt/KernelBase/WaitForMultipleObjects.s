.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_WaitForMultipleObjects,WaitForMultipleObjects,1777

	.text.windows
WaitForMultipleObjects:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WaitForMultipleObjects(%rip),%rax
	jmp	__sysv2nt
	.endfn	WaitForMultipleObjects,globl
	.previous
