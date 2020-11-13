.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_RegisterClassExA,RegisterClassExA,2249

	.text.windows
RegisterClassExA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_RegisterClassExA(%rip)
	leave
	ret
	.endfn	RegisterClassExA,globl
	.previous
