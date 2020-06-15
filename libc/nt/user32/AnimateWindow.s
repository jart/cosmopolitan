.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_AnimateWindow,AnimateWindow,1513

	.text.windows
AnimateWindow:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_AnimateWindow(%rip),%rax
	jmp	__sysv2nt
	.endfn	AnimateWindow,globl
	.previous
