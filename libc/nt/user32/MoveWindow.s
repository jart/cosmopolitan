.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_MoveWindow,MoveWindow,2176

	.text.windows
MoveWindow:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_MoveWindow(%rip),%rax
	jmp	__sysv2nt6
	.endfn	MoveWindow,globl
	.previous
