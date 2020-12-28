.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_InitializeCriticalSectionAndSpinCount,InitializeCriticalSectionAndSpinCount,0

	.text.windows
InitializeCriticalSectionAndSpinCount:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_InitializeCriticalSectionAndSpinCount(%rip),%rax
	jmp	__sysv2nt
	.endfn	InitializeCriticalSectionAndSpinCount,globl
	.previous
