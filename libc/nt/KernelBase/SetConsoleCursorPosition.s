.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_SetConsoleCursorPosition,SetConsoleCursorPosition,1476

	.text.windows
SetConsoleCursorPosition:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetConsoleCursorPosition(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetConsoleCursorPosition,globl
	.previous
