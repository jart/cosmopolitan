#include "libc/nt/ntdllimport.h"
.ntimp	CsrClientCallServer,CsrClientCallServer

#ifdef __x86_64__
	.text.windows
CsrClientCallServer:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CsrClientCallServer(%rip),%rax
	jmp	__sysv2nt
	.endfn	CsrClientCallServer,globl
	.previous
#endif
