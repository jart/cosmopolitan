.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_GetCursorPos,GetCursorPos,1829

	.text.windows
GetCursorPos:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetCursorPos(%rip)
	leave
	ret
	.endfn	GetCursorPos,globl
	.previous
