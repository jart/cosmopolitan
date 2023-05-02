#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSAWaitForMultipleEvents,WSAWaitForMultipleEvents,124

#ifdef __x86_64__
	.text.windows
__WSAWaitForMultipleEvents:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSAWaitForMultipleEvents(%rip),%rax
	jmp	__sysv2nt6
	.endfn	__WSAWaitForMultipleEvents,globl
	.previous
#endif
