.include "o/libc/nt/codegen.inc"
.imp	gdi32,__imp_SelectObject,SelectObject,1864

	.text.windows
SelectObject:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SelectObject(%rip),%rax
	jmp	__sysv2nt
	.endfn	SelectObject,globl
	.previous
