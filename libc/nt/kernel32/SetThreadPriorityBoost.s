.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_SetThreadPriorityBoost,SetThreadPriorityBoost,0

	.text.windows
SetThreadPriorityBoost:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetThreadPriorityBoost(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetThreadPriorityBoost,globl
	.previous
