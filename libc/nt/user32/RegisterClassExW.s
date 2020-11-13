.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_RegisterClassExW,RegisterClassExW,2250

	.text.windows
RegisterClassEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_RegisterClassExW(%rip)
	leave
	ret
	.endfn	RegisterClassEx,globl
	.previous
