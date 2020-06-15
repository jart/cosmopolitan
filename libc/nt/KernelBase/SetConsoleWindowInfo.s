.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_SetConsoleWindowInfo,SetConsoleWindowInfo,1490

	.text.windows
SetConsoleWindowInfo:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetConsoleWindowInfo(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetConsoleWindowInfo,globl
	.previous
