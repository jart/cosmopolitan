.include "o/libc/nt/codegen.inc"
.imp	gdi32,__imp_CreateBitmap,CreateBitmap,1056

	.text.windows
CreateBitmap:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateBitmap(%rip),%rax
	jmp	__sysv2nt6
	.endfn	CreateBitmap,globl
	.previous
