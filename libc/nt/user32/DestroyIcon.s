.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_DestroyIcon,DestroyIcon,1687

	.text.windows
DestroyIcon:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_DestroyIcon(%rip)
	leave
	ret
	.endfn	DestroyIcon,globl
	.previous
