.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_SetThreadPriority,SetThreadPriority,1565

	.text.windows
SetThreadPriority:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetThreadPriority(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetThreadPriority,globl
	.previous
