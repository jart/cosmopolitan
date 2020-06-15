.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_WriteConsoleInputA,WriteConsoleInputA,1815

	.text.windows
WriteConsoleInputA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WriteConsoleInputA(%rip),%rax
	jmp	__sysv2nt
	.endfn	WriteConsoleInputA,globl
	.previous
