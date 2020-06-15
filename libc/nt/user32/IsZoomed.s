.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_IsZoomed,IsZoomed,2092

	.text.windows
IsZoomed:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_IsZoomed(%rip)
	leave
	ret
	.endfn	IsZoomed,globl
	.previous
