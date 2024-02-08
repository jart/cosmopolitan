#include "libc/nt/codegen.h"
.imp	iphlpapi,__imp_FlushIpNetTable,FlushIpNetTable

	.text.windows
	.ftrace1
FlushIpNetTable:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_FlushIpNetTable(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	FlushIpNetTable,globl
	.previous
