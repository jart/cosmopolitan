.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_GetMenu,GetMenu,1881

	.text.windows
GetMenu:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetMenu(%rip)
	leave
	ret
	.endfn	GetMenu,globl
	.previous
