.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_DestroyWindow,DestroyWindow,1690

	.text.windows
DestroyWindow:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_DestroyWindow(%rip)
	leave
	ret
	.endfn	DestroyWindow,globl
	.previous
