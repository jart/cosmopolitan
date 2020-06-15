.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GetProcessHandleCount,GetProcessHandleCount,671

	.text.windows
GetProcessHandleCount:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetProcessHandleCount(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetProcessHandleCount,globl
	.previous
