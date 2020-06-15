.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetProcessWorkingSetSize,GetProcessWorkingSetSize,708

	.text.windows
GetProcessWorkingSetSize:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetProcessWorkingSetSize(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetProcessWorkingSetSize,globl
	.previous
