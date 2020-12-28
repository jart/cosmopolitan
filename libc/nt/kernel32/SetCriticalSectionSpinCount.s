.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_SetCriticalSectionSpinCount,SetCriticalSectionSpinCount,0

	.text.windows
SetCriticalSectionSpinCount:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetCriticalSectionSpinCount(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetCriticalSectionSpinCount,globl
	.previous
