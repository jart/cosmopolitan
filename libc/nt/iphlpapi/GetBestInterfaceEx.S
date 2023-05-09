#include "libc/nt/codegen.h"
.imp	iphlpapi,__imp_GetBestInterfaceEx,GetBestInterfaceEx,0

	.text.windows
GetBestInterfaceEx:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetBestInterfaceEx(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	GetBestInterfaceEx,globl
	.previous
