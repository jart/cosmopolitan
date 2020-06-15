.include "o/libc/nt/ntdllimport.inc"
.ntimp	CsrClientCallServer

	.text.windows
CsrClientCallServer:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CsrClientCallServer(%rip),%rax
	jmp	__sysv2nt
	.endfn	CsrClientCallServer,globl
	.previous
