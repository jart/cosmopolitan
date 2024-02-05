#include "libc/nt/codegen.h"
.imp	iphlpapi,__imp_GetBestRoute,GetBestRoute

	.text.windows
	.ftrace1
GetBestRoute:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_GetBestRoute(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	GetBestRoute,globl
	.previous
