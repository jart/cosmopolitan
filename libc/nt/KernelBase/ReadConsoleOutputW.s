.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_ReadConsoleOutputW,ReadConsoleOutputW,1307

	.text.windows
ReadConsoleOutput:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_ReadConsoleOutputW(%rip),%rax
	jmp	__sysv2nt6
	.endfn	ReadConsoleOutput,globl
	.previous
