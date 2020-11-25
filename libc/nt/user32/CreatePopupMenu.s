.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_CreatePopupMenu,CreatePopupMenu,1622

	.text.windows
CreatePopupMenu:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_CreatePopupMenu(%rip)
	leave
	ret
	.endfn	CreatePopupMenu,globl
	.previous
