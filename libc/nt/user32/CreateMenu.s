.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_CreateMenu,CreateMenu,1621

	.text.windows
CreateMenu:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_CreateMenu(%rip)
	leave
	ret
	.endfn	CreateMenu,globl
	.previous
