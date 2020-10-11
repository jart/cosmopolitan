.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_CreateWaitableTimerW,CreateWaitableTimerW,234

	.text.windows
CreateWaitableTimer:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateWaitableTimerW(%rip),%rax
	jmp	__sysv2nt
	.endfn	CreateWaitableTimer,globl
	.previous
