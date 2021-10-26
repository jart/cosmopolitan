.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_CreateIconIndirect,CreateIconIndirect,1618

	.text.windows
CreateIconIndirect:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_CreateIconIndirect(%rip)
	leave
	ret
	.endfn	CreateIconIndirect,globl
	.previous
