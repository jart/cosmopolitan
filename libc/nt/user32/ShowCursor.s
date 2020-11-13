.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_ShowCursor,ShowCursor,2412

	.text.windows
ShowCursor:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_ShowCursor(%rip)
	leave
	ret
	.endfn	ShowCursor,globl
	.previous
