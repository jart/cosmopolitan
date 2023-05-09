#include "libc/nt/ntdllimport.h"
.ntimp	CsrClientCallServer,CsrClientCallServer

	.text.windows
CsrClientCallServer:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CsrClientCallServer(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	CsrClientCallServer,globl
	.previous
