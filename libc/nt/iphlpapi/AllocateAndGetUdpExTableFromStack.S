#include "libc/nt/codegen.h"
.imp	iphlpapi,__imp_AllocateAndGetUdpExTableFromStack,AllocateAndGetUdpExTableFromStack,0

#ifdef __x86_64__
	.text.windows
AllocateAndGetUdpExTableFromStack:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_AllocateAndGetUdpExTableFromStack(%rip),%rax
	jmp	__sysv2nt6
	.endfn	AllocateAndGetUdpExTableFromStack,globl
	.previous
#endif
