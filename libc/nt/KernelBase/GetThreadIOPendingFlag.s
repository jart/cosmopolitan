.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GetThreadIOPendingFlag,GetThreadIOPendingFlag,768

	.text.windows
GetThreadIOPendingFlag:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetThreadIOPendingFlag(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetThreadIOPendingFlag,globl
	.previous
