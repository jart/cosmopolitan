.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_MessageBoxExA,MessageBoxExA,2164

	.text.windows
MessageBoxExA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_MessageBoxExA(%rip),%rax
	jmp	__sysv2nt6
	.endfn	MessageBoxExA,globl
	.previous
