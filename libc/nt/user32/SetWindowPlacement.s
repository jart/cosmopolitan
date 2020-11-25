.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_SetWindowPlacement,SetWindowPlacement,2399

	.text.windows
SetWindowPlacement:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetWindowPlacement(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetWindowPlacement,globl
	.previous
