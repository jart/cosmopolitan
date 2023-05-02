#include "libc/nt/codegen.h"
.imp	iphlpapi,__imp_GetTcpTable,GetTcpTable,0

#ifdef __x86_64__
	.text.windows
GetTcpTable:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetTcpTable(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetTcpTable,globl
	.previous
#endif
