#include "libc/nt/codegen.h"
.imp	iphlpapi,__imp_GetAdapterIndex,GetAdapterIndex,0

	.text.windows
GetAdapterIndex:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetAdapterIndex(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	GetAdapterIndex,globl
	.previous
