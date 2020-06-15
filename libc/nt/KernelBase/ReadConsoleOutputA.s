.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_ReadConsoleOutputA,ReadConsoleOutputA,1303

	.text.windows
ReadConsoleOutputA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_ReadConsoleOutputA(%rip),%rax
	jmp	__sysv2nt6
	.endfn	ReadConsoleOutputA,globl
	.previous
