.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GetConsoleCursorInfo,GetConsoleCursorInfo,465

	.text.windows
GetConsoleCursorInfo:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetConsoleCursorInfo(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetConsoleCursorInfo,globl
	.previous
