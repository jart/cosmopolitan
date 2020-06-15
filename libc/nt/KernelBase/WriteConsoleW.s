.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_WriteConsoleW,WriteConsoleW,1822

	.text.windows
WriteConsole:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WriteConsoleW(%rip),%rax
	jmp	__sysv2nt6
	.endfn	WriteConsole,globl
	.previous
