.include "o/libc/nt/codegen.inc"
.imp	gdi32,__imp_ChoosePixelFormat,ChoosePixelFormat,1040

	.text.windows
ChoosePixelFormat:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_ChoosePixelFormat(%rip),%rax
	jmp	__sysv2nt
	.endfn	ChoosePixelFormat,globl
	.previous
