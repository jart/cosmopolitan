.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_GetParent,GetParent,1906

	.text.windows
GetParent:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetParent(%rip)
	leave
	ret
	.endfn	GetParent,globl
	.previous
