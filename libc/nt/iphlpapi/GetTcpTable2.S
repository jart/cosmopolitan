#include "libc/nt/codegen.h"
.imp	iphlpapi,__imp_GetTcpTable2,GetTcpTable2

	.text.windows
	.ftrace1
GetTcpTable2:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_GetTcpTable2(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	GetTcpTable2,globl
	.previous
