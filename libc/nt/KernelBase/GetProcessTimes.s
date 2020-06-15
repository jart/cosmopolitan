.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GetProcessTimes,GetProcessTimes,684

	.text.windows
GetProcessTimes:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetProcessTimes(%rip),%rax
	jmp	__sysv2nt6
	.endfn	GetProcessTimes,globl
	.previous
