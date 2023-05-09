#include "libc/nt/codegen.h"
.imp	iphlpapi,__imp_GetBestRoute,GetBestRoute,0

	.text.windows
GetBestRoute:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetBestRoute(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	GetBestRoute,globl
	.previous
