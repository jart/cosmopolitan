.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GetPriorityClass,GetPriorityClass,665

	.text.windows
GetPriorityClass:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetPriorityClass(%rip)
	leave
	ret
	.endfn	GetPriorityClass,globl
	.previous
