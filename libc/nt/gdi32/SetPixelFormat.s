.include "o/libc/nt/codegen.inc"
.imp	gdi32,__imp_SetPixelFormat,SetPixelFormat,1902

	.text.windows
SetPixelFormat:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetPixelFormat(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetPixelFormat,globl
	.previous
