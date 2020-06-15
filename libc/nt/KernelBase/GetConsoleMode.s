.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GetConsoleMode,GetConsoleMode,471

	.text.windows
GetConsoleMode:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetConsoleMode(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetConsoleMode,globl
	.previous
