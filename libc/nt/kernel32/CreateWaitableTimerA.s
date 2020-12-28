.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_CreateWaitableTimerA,CreateWaitableTimerA,253

	.text.windows
CreateWaitableTimerA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateWaitableTimerA(%rip),%rax
	jmp	__sysv2nt
	.endfn	CreateWaitableTimerA,globl
	.previous
