#include "libc/nt/codegen.h"
.imp	iphlpapi,__imp_GetAdaptersAddresses,GetAdaptersAddresses

	.text.windows
	.ftrace1
GetAdaptersAddresses:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_GetAdaptersAddresses(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	GetAdaptersAddresses,globl
	.previous
