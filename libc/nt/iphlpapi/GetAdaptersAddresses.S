#include "libc/nt/codegen.h"
.imp	iphlpapi,__imp_GetAdaptersAddresses,GetAdaptersAddresses,67

#ifdef __x86_64__
	.text.windows
GetAdaptersAddresses:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetAdaptersAddresses(%rip),%rax
	jmp	__sysv2nt6
	.endfn	GetAdaptersAddresses,globl
	.previous
#endif
