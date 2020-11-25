.include "o/libc/nt/codegen.inc"
.imp	gdi32,__imp_SetPixel,SetPixel,1901

	.text.windows
SetPixel:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetPixel(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetPixel,globl
	.previous
