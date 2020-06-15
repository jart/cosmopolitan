.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_IsWindow,IsWindow,2084

	.text.windows
IsWindow:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_IsWindow(%rip)
	leave
	ret
	.endfn	IsWindow,globl
	.previous
