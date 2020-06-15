.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_MessageBoxW,MessageBoxW,2170

	.text.windows
MessageBox:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_MessageBoxW(%rip),%rax
	jmp	__sysv2nt
	.endfn	MessageBox,globl
	.previous
