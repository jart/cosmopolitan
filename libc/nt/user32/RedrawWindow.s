.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_RedrawWindow,RedrawWindow,2246

	.text.windows
RedrawWindow:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RedrawWindow(%rip),%rax
	jmp	__sysv2nt
	.endfn	RedrawWindow,globl
	.previous
