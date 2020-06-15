.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_GetWindowTextA,GetWindowTextA,2003

	.text.windows
GetWindowTextA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetWindowTextA(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetWindowTextA,globl
	.previous
