#include "libc/nt/codegen.h"
.imp	iphlpapi,__imp_GetTcpTable2,GetTcpTable2,0

#ifdef __x86_64__
	.text.windows
GetTcpTable2:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetTcpTable2(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetTcpTable2,globl
	.previous
#endif
