.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetConsoleScreenBufferInfoEx,GetConsoleScreenBufferInfoEx,0

	.text.windows
GetConsoleScreenBufferInfoEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetConsoleScreenBufferInfoEx(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetConsoleScreenBufferInfoEx,globl
	.previous
