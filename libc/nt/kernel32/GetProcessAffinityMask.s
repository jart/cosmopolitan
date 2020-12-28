.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetProcessAffinityMask,GetProcessAffinityMask,0

	.text.windows
GetProcessAffinityMask:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetProcessAffinityMask(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetProcessAffinityMask,globl
	.previous
