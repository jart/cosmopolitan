.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_SetWindowTextA,SetWindowTextA,2404

	.text.windows
SetWindowTextA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetWindowTextA(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetWindowTextA,globl
	.previous
