#include "libc/nt/codegen.h"
.imp	iphlpapi,__imp_GetBestInterfaceEx,GetBestInterfaceEx,0

#ifdef __x86_64__
	.text.windows
GetBestInterfaceEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetBestInterfaceEx(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetBestInterfaceEx,globl
	.previous
#endif
