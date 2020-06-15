.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_SetWindowPos,SetWindowPos,2400

	.text.windows
SetWindowPos:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetWindowPos(%rip),%rax
	jmp	__sysv2nt8
	.endfn	SetWindowPos,globl
	.previous
