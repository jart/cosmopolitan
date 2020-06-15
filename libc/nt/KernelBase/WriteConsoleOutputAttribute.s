.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_WriteConsoleOutputAttribute,WriteConsoleOutputAttribute,1818

	.text.windows
WriteConsoleOutputAttribute:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WriteConsoleOutputAttribute(%rip),%rax
	jmp	__sysv2nt6
	.endfn	WriteConsoleOutputAttribute,globl
	.previous
