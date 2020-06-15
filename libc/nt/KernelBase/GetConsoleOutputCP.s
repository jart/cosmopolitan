.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GetConsoleOutputCP,GetConsoleOutputCP,474

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
