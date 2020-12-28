.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_SetConsoleOutputCP,SetConsoleOutputCP,0

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
