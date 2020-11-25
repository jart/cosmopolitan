.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_GetWindowPlacement,GetWindowPlacement,1995

	.text.windows
GetWindowPlacement:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetWindowPlacement(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetWindowPlacement,globl
	.previous
