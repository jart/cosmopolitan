.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_DestroyMenu,DestroyMenu,1688

	.text.windows
DestroyMenu:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_DestroyMenu(%rip)
	leave
	ret
	.endfn	DestroyMenu,globl
	.previous
