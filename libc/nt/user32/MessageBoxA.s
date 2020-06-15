.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_MessageBoxA,MessageBoxA,2163

	.text.windows
MessageBoxA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_MessageBoxA(%rip),%rax
	jmp	__sysv2nt
	.endfn	MessageBoxA,globl
	.previous
