.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_SleepEx,SleepEx,0

	.text.windows
SleepEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SleepEx(%rip),%rax
	jmp	__sysv2nt
	.endfn	SleepEx,globl
	.previous
