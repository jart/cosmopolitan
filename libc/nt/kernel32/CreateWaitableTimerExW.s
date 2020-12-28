.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_CreateWaitableTimerExW,CreateWaitableTimerExW,0

	.text.windows
CreateWaitableTimerEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateWaitableTimerExW(%rip),%rax
	jmp	__sysv2nt
	.endfn	CreateWaitableTimerEx,globl
	.previous
