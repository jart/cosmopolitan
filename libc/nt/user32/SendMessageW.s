.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_SendMessageW,SendMessageW,2306

	.text.windows
SendMessage:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SendMessageW(%rip),%rax
	jmp	__sysv2nt
	.endfn	SendMessage,globl
	.previous
