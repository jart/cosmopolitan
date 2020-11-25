.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_SetClassLongW,SetClassLongW,2316

	.text.windows
SetClassLong:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetClassLongW(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetClassLong,globl
	.previous
