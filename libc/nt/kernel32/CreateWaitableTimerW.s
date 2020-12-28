.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_CreateWaitableTimerW,CreateWaitableTimerW,0

	.text.windows
CreateWaitableTimer:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateWaitableTimerW(%rip),%rax
	jmp	__sysv2nt
	.endfn	CreateWaitableTimer,globl
	.previous
