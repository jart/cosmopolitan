.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_SetProcessAffinityMask,SetProcessAffinityMask,1347

	.text.windows
SetProcessAffinityMask:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetProcessAffinityMask(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetProcessAffinityMask,globl
	.previous
