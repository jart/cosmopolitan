.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GetThreadTimes,GetThreadTimes,777

	.text.windows
GetThreadTimes:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetThreadTimes(%rip),%rax
	jmp	__sysv2nt6
	.endfn	GetThreadTimes,globl
	.previous
