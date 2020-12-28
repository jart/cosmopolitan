.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_ReadConsoleOutputA,ReadConsoleOutputA,0

	.text.windows
ReadConsoleOutputA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_ReadConsoleOutputA(%rip),%rax
	jmp	__sysv2nt6
	.endfn	ReadConsoleOutputA,globl
	.previous
