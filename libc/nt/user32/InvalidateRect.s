.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_InvalidateRect,InvalidateRect,2048

	.text.windows
InvalidateRect:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_InvalidateRect(%rip),%rax
	jmp	__sysv2nt
	.endfn	InvalidateRect,globl
	.previous
