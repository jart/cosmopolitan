.include "o/libc/nt/codegen.inc"
.imp	gdi32,__imp_CreateRectRgn,CreateRectRgn,1097

	.text.windows
CreateRectRgn:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateRectRgn(%rip),%rax
	jmp	__sysv2nt
	.endfn	CreateRectRgn,globl
	.previous
