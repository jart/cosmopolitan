#include "libc/nt/codegen.h"
.imp	iphlpapi,__imp_GetAdaptersInfo,GetAdaptersInfo

	.text.windows
	.ftrace1
GetAdaptersInfo:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_GetAdaptersInfo(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	GetAdaptersInfo,globl
	.previous
