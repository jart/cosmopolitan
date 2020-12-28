.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetThreadIOPendingFlag,GetThreadIOPendingFlag,0

	.text.windows
GetThreadIOPendingFlag:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetThreadIOPendingFlag(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetThreadIOPendingFlag,globl
	.previous
