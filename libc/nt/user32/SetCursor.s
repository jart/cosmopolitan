.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_SetCursor,SetCursor,2321

	.text.windows
SetCursor:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_SetCursor(%rip)
	leave
	ret
	.endfn	SetCursor,globl
	.previous
