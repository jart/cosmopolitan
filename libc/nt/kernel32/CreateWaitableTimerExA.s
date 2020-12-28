.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_CreateWaitableTimerExA,CreateWaitableTimerExA,254

	.text.windows
CreateWaitableTimerExA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateWaitableTimerExA(%rip),%rax
	jmp	__sysv2nt
	.endfn	CreateWaitableTimerExA,globl
	.previous
