#include "libc/nt/codegen.h"
.imp	iphlpapi,__imp_CreateIpForwardEntry,CreateIpForwardEntry

	.text.windows
	.ftrace1
CreateIpForwardEntry:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_CreateIpForwardEntry(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	CreateIpForwardEntry,globl
	.previous
