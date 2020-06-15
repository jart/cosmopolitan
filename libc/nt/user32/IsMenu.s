.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_IsMenu,IsMenu,2070

	.text.windows
IsMenu:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_IsMenu(%rip)
	leave
	ret
	.endfn	IsMenu,globl
	.previous
