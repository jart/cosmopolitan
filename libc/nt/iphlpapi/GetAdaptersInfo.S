#include "libc/nt/codegen.h"
.imp	iphlpapi,__imp_GetAdaptersInfo,GetAdaptersInfo,0

	.text.windows
GetAdaptersInfo:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetAdaptersInfo(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	GetAdaptersInfo,globl
	.previous
