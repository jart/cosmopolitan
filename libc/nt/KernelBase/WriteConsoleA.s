.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_WriteConsoleA,WriteConsoleA,1814

	.text.windows
WriteConsoleA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WriteConsoleA(%rip),%rax
	jmp	__sysv2nt6
	.endfn	WriteConsoleA,globl
	.previous
