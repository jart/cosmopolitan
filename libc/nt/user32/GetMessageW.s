.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_GetMessageW,GetMessageW,1899

	.text.windows
GetMessage:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetMessageW(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetMessage,globl
	.previous
