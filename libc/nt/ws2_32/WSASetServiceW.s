.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_WSASetServiceW,WSASetServiceW,118

	.text.windows
WSASetService:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSASetServiceW(%rip),%rax
	jmp	__sysv2nt
	.endfn	WSASetService,globl
	.previous
