.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetConsoleScreenBufferInfo,GetConsoleScreenBufferInfo,0

	.text.windows
GetConsoleScreenBufferInfo:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetConsoleScreenBufferInfo(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetConsoleScreenBufferInfo,globl
	.previous
