.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_FillConsoleOutputAttribute,FillConsoleOutputAttribute,354

	.text.windows
FillConsoleOutputAttribute:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_FillConsoleOutputAttribute(%rip),%rax
	jmp	__sysv2nt6
	.endfn	FillConsoleOutputAttribute,globl
	.previous
