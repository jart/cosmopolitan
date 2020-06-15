.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_SetPriorityClass,SetPriorityClass,1526

	.text.windows
SetPriorityClass:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetPriorityClass(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetPriorityClass,globl
	.previous
