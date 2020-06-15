.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_WSAGetQOSByName,WSAGetQOSByName,69

	.text.windows
WSAGetQOSByName:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSAGetQOSByName(%rip),%rax
	jmp	__sysv2nt
	.endfn	WSAGetQOSByName,globl
	.previous
