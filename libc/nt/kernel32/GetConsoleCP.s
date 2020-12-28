.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetConsoleCP,GetConsoleCP,0

	.text.windows
GetConsoleCP:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_GetConsoleCP(%rip)
	leave
	ret
	.endfn	GetConsoleCP,globl
	.previous
