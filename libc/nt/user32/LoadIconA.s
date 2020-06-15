.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_LoadIconA,LoadIconA,2102

	.text.windows
LoadIconA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_LoadIconA(%rip),%rax
	jmp	__sysv2nt
	.endfn	LoadIconA,globl
	.previous
