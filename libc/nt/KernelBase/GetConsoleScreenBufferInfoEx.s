.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GetConsoleScreenBufferInfoEx,GetConsoleScreenBufferInfoEx,477

	.text.windows
GetConsoleScreenBufferInfoEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetConsoleScreenBufferInfoEx(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetConsoleScreenBufferInfoEx,globl
	.previous
