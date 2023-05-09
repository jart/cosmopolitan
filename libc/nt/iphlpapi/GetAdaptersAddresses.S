#include "libc/nt/codegen.h"
.imp	iphlpapi,__imp_GetAdaptersAddresses,GetAdaptersAddresses,67

	.text.windows
GetAdaptersAddresses:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetAdaptersAddresses(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	GetAdaptersAddresses,globl
	.previous
