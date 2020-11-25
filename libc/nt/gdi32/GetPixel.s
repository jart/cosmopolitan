.include "o/libc/nt/codegen.inc"
.imp	gdi32,__imp_GetPixel,GetPixel,1691

	.text.windows
GetPixel:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetPixel(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetPixel,globl
	.previous
