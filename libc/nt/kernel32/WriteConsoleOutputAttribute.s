.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_WriteConsoleOutputAttribute,WriteConsoleOutputAttribute,0

	.text.windows
WriteConsoleOutputAttribute:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WriteConsoleOutputAttribute(%rip),%rax
	jmp	__sysv2nt6
	.endfn	WriteConsoleOutputAttribute,globl
	.previous
