.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_SetClassLongA,SetClassLongA,2313

	.text.windows
SetClassLongA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetClassLongA(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetClassLongA,globl
	.previous
