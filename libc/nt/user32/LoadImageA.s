.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_LoadImageA,LoadImageA,2104

	.text.windows
LoadImageA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_LoadImageA(%rip),%rax
	jmp	__sysv2nt6
	.endfn	LoadImageA,globl
	.previous
