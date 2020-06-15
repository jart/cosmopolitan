.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GetConsoleWindow,GetConsoleWindow,481

	.text.windows
GetConsoleWindow:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_GetConsoleWindow(%rip)
	leave
	ret
	.endfn	GetConsoleWindow,globl
	.previous
