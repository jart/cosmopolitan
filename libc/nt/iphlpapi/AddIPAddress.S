#include "libc/nt/codegen.h"
.imp	iphlpapi,__imp_AddIPAddress,AddIPAddress,0

#ifdef __x86_64__
	.text.windows
AddIPAddress:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_AddIPAddress(%rip),%rax
	jmp	__sysv2nt6
	.endfn	AddIPAddress,globl
	.previous
#endif
