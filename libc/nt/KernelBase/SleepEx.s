.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_SleepEx,SleepEx,1593

	.text.windows
SleepEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SleepEx(%rip),%rax
	jmp	__sysv2nt
	.endfn	SleepEx,globl
	.previous
