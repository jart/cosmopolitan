.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_SetConsoleOutputCP,SetConsoleOutputCP,1484

	.text.windows
SetConsoleOutputCP:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_SetConsoleOutputCP(%rip)
	leave
	ret
	.endfn	SetConsoleOutputCP,globl
	.previous
