.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_SetConsoleScreenBufferSize,SetConsoleScreenBufferSize,0

	.text.windows
SetConsoleScreenBufferSize:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetConsoleScreenBufferSize(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetConsoleScreenBufferSize,globl
	.previous
