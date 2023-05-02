#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSASend,WSASend,96

#ifdef __x86_64__
	.text.windows
WSASend:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSASend(%rip),%rax
	jmp	__sysv2nt8
	.endfn	WSASend,globl
	.previous
#endif
