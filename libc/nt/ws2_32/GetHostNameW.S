#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_GetHostNameW,GetHostNameW,33

#ifdef __x86_64__
	.text.windows
GetHostName:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetHostNameW(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetHostName,globl
	.previous
#endif
