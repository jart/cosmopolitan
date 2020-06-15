.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_WaitForSingleObjectEx,WaitForSingleObjectEx,1780

	.text.windows
WaitForSingleObjectEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WaitForSingleObjectEx(%rip),%rax
	jmp	__sysv2nt
	.endfn	WaitForSingleObjectEx,globl
	.previous
