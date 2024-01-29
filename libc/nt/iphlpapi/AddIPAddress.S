#include "libc/nt/codegen.h"
.imp	iphlpapi,__imp_AddIPAddress,AddIPAddress

	.text.windows
	.ftrace1
AddIPAddress:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_AddIPAddress(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	AddIPAddress,globl
	.previous
