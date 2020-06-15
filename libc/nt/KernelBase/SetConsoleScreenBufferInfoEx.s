.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_SetConsoleScreenBufferInfoEx,SetConsoleScreenBufferInfoEx,1485

	.text.windows
SetConsoleScreenBufferInfoEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetConsoleScreenBufferInfoEx(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetConsoleScreenBufferInfoEx,globl
	.previous
