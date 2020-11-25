.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_GetSystemMenu,GetSystemMenu,1955

	.text.windows
GetSystemMenu:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetSystemMenu(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetSystemMenu,globl
	.previous
