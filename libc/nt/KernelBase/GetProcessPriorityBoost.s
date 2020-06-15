.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GetProcessPriorityBoost,GetProcessPriorityBoost,682

	.text.windows
GetProcessPriorityBoost:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetProcessPriorityBoost(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetProcessPriorityBoost,globl
	.previous
