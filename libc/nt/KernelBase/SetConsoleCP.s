.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_SetConsoleCP,SetConsoleCP,1473

	.text.windows
SetConsoleCP:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_SetConsoleCP(%rip)
	leave
	ret
	.endfn	SetConsoleCP,globl
	.previous
