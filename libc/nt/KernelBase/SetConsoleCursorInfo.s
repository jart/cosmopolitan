.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_SetConsoleCursorInfo,SetConsoleCursorInfo,1475

	.text.windows
SetConsoleCursorInfo:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetConsoleCursorInfo(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetConsoleCursorInfo,globl
	.previous
