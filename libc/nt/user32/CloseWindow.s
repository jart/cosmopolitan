.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_CloseWindow,CloseWindow,1591

	.text.windows
CloseWindow:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_CloseWindow(%rip)
	leave
	ret
	.endfn	CloseWindow,globl
	.previous
