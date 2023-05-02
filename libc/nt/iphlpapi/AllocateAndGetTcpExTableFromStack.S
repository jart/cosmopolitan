#include "libc/nt/codegen.h"
.imp	iphlpapi,__imp_AllocateAndGetTcpExTableFromStack,AllocateAndGetTcpExTableFromStack,0

#ifdef __x86_64__
	.text.windows
AllocateAndGetTcpExTableFromStack:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_AllocateAndGetTcpExTableFromStack(%rip),%rax
	jmp	__sysv2nt6
	.endfn	AllocateAndGetTcpExTableFromStack,globl
	.previous
#endif
