.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_ShowWindow,ShowWindow,2417

	.text.windows
ShowWindow:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_ShowWindow(%rip),%rax
	jmp	__sysv2nt
	.endfn	ShowWindow,globl
	.previous
