.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_WaitForMultipleObjectsEx,WaitForMultipleObjectsEx,1778

	.text.windows
WaitForMultipleObjectsEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WaitForMultipleObjectsEx(%rip),%rax
	jmp	__sysv2nt6
	.endfn	WaitForMultipleObjectsEx,globl
	.previous
