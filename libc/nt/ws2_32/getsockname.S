#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_getsockname,getsockname

	.text.windows
        .ftrace1
__sys_getsockname_nt:
        .ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_getsockname(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	__sys_getsockname_nt,globl
	.previous
