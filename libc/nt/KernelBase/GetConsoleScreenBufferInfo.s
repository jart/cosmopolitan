.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GetConsoleScreenBufferInfo,GetConsoleScreenBufferInfo,476

	.text.windows
GetConsoleScreenBufferInfo:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetConsoleScreenBufferInfo(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetConsoleScreenBufferInfo,globl
	.previous
