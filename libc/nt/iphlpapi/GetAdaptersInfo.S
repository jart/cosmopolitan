#include "libc/nt/codegen.h"
.imp	iphlpapi,__imp_GetAdaptersInfo,GetAdaptersInfo,0

#ifdef __x86_64__
	.text.windows
GetAdaptersInfo:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetAdaptersInfo(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetAdaptersInfo,globl
	.previous
#endif
