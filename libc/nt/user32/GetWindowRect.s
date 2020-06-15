.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_GetWindowRect,GetWindowRect,1997

	.text.windows
GetWindowRect:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetWindowRect(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetWindowRect,globl
	.previous
