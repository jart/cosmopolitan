.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_GetDC,GetDC,1830

	.text.windows
GetDC:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetDC(%rip)
	leave
	ret
	.endfn	GetDC,globl
	.previous
