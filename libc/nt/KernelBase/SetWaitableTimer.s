.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_SetWaitableTimer,SetWaitableTimer,1583

	.text.windows
SetWaitableTimer:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetWaitableTimer(%rip),%rax
	jmp	__sysv2nt6
	.endfn	SetWaitableTimer,globl
	.previous
