.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_ReadConsoleW,ReadConsoleW,1308

	.text.windows
ReadConsole:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_ReadConsoleW(%rip),%rax
	jmp	__sysv2nt6
	.endfn	ReadConsole,globl
	.previous
