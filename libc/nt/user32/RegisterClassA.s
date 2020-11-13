.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_RegisterClassA,RegisterClassA,2248

	.text.windows
RegisterClassA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_RegisterClassA(%rip)
	leave
	ret
	.endfn	RegisterClassA,globl
	.previous
