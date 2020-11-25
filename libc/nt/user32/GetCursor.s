.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_GetCursor,GetCursor,1826

	.text.windows
GetCursor:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_GetCursor(%rip)
	leave
	ret
	.endfn	GetCursor,globl
	.previous
