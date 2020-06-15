.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_WriteConsoleInputW,WriteConsoleInputW,1816

	.text.windows
WriteConsoleInput:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WriteConsoleInputW(%rip),%rax
	jmp	__sysv2nt
	.endfn	WriteConsoleInput,globl
	.previous
