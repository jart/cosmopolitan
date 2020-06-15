.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_SetThreadAffinityMask,SetThreadAffinityMask,1369

	.text.windows
SetThreadAffinityMask:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetThreadAffinityMask(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetThreadAffinityMask,globl
	.previous
