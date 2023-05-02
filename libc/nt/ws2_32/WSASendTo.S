#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSASendTo,WSASendTo,99

#ifdef __x86_64__
	.text.windows
WSASendTo:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSASendTo(%rip),%rax
	jmp	__sysv2nt10
	.endfn	WSASendTo,globl
	.previous
#endif
