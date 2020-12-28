.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_SetConsoleMode,SetConsoleMode,0

	.text.windows
SetConsoleMode:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetConsoleMode(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetConsoleMode,globl
	.previous
