.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_SetProcessPriorityBoost,SetProcessPriorityBoost,1535

	.text.windows
SetProcessPriorityBoost:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetProcessPriorityBoost(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetProcessPriorityBoost,globl
	.previous
