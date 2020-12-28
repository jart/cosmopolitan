.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetConsoleCursorInfo,GetConsoleCursorInfo,0

	.text.windows
GetConsoleCursorInfo:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetConsoleCursorInfo(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetConsoleCursorInfo,globl
	.previous
