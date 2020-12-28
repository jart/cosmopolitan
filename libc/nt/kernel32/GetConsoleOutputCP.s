.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetConsoleOutputCP,GetConsoleOutputCP,0

	.text.windows
GetConsoleOutputCP:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_GetConsoleOutputCP(%rip)
	leave
	ret
	.endfn	GetConsoleOutputCP,globl
	.previous
