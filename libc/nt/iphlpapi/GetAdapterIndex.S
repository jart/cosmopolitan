#include "libc/nt/codegen.h"
.imp	iphlpapi,__imp_GetAdapterIndex,GetAdapterIndex,0

#ifdef __x86_64__
	.text.windows
GetAdapterIndex:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetAdapterIndex(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetAdapterIndex,globl
	.previous
#endif
