#include "libc/nt/codegen.h"
.imp	iphlpapi,__imp_GetBestRoute,GetBestRoute,0

#ifdef __x86_64__
	.text.windows
GetBestRoute:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetBestRoute(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetBestRoute,globl
	.previous
#endif
