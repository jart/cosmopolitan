.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_SetPriorityClass,SetPriorityClass,0

	.text.windows
SetPriorityClass:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetPriorityClass(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetPriorityClass,globl
	.previous
