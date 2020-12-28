.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_ReadConsoleOutputAttribute,ReadConsoleOutputAttribute,0

	.text.windows
ReadConsoleOutputAttribute:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_ReadConsoleOutputAttribute(%rip),%rax
	jmp	__sysv2nt6
	.endfn	ReadConsoleOutputAttribute,globl
	.previous
