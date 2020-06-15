.include "o/libc/nt/codegen.inc"
.imp	MsWSock,__imp_GetAcceptExSockaddrs,GetAcceptExSockaddrs,4

	.text.windows
GetAcceptExSockaddrs:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetAcceptExSockaddrs(%rip),%rax
	jmp	__sysv2nt8
	.endfn	GetAcceptExSockaddrs,globl
	.previous
