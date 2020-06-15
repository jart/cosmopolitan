.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_WSAAccept,WSAAccept,41

	.text.windows
WSAAccept:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSAAccept(%rip),%rax
	jmp	__sysv2nt6
	.endfn	WSAAccept,globl
	.previous
