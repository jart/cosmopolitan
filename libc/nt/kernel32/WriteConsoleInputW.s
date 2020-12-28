.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_WriteConsoleInputW,WriteConsoleInputW,0

	.text.windows
WriteConsoleInput:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WriteConsoleInputW(%rip),%rax
	jmp	__sysv2nt
	.endfn	WriteConsoleInput,globl
	.previous
