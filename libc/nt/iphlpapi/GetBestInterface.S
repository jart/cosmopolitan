#include "libc/nt/codegen.h"
.imp	iphlpapi,__imp_GetBestInterface,GetBestInterface,0

#ifdef __x86_64__
	.text.windows
GetBestInterface:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetBestInterface(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetBestInterface,globl
	.previous
#endif
