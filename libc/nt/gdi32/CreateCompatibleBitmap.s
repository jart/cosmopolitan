.include "o/libc/nt/codegen.inc"
.imp	gdi32,__imp_CreateCompatibleBitmap,CreateCompatibleBitmap,1063

	.text.windows
CreateCompatibleBitmap:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateCompatibleBitmap(%rip),%rax
	jmp	__sysv2nt
	.endfn	CreateCompatibleBitmap,globl
	.previous
