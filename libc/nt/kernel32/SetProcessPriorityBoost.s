.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_SetProcessPriorityBoost,SetProcessPriorityBoost,0

	.text.windows
SetProcessPriorityBoost:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetProcessPriorityBoost(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetProcessPriorityBoost,globl
	.previous
