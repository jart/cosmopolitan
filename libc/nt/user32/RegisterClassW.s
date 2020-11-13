.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_RegisterClassW,RegisterClassW,2251

	.text.windows
RegisterClass:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_RegisterClassW(%rip)
	leave
	ret
	.endfn	RegisterClass,globl
	.previous
