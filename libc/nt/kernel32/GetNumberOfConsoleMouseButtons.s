.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetNumberOfConsoleMouseButtons,GetNumberOfConsoleMouseButtons,0

	.text.windows
GetNumberOfConsoleMouseButtons:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetNumberOfConsoleMouseButtons(%rip)
	leave
	ret
	.endfn	GetNumberOfConsoleMouseButtons,globl
	.previous
