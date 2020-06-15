.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_GetWindow,GetWindow,1976

	.text.windows
GetWindow:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetWindow(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetWindow,globl
	.previous
