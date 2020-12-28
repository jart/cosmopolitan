.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_FillConsoleOutputAttribute,FillConsoleOutputAttribute,0

	.text.windows
FillConsoleOutputAttribute:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_FillConsoleOutputAttribute(%rip),%rax
	jmp	__sysv2nt6
	.endfn	FillConsoleOutputAttribute,globl
	.previous
