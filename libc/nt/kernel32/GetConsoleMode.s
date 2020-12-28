.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetConsoleMode,GetConsoleMode,0

	.text.windows
GetConsoleMode:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetConsoleMode(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetConsoleMode,globl
	.previous
