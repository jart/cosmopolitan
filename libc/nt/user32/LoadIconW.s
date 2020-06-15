.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_LoadIconW,LoadIconW,2103

	.text.windows
LoadIcon:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_LoadIconW(%rip),%rax
	jmp	__sysv2nt
	.endfn	LoadIcon,globl
	.previous
