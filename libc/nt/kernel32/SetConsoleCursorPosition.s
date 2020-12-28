.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_SetConsoleCursorPosition,SetConsoleCursorPosition,0

	.text.windows
SetConsoleCursorPosition:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetConsoleCursorPosition(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetConsoleCursorPosition,globl
	.previous
