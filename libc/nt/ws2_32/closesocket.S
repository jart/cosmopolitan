#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_closesocket,closesocket

	.text.windows
        .ftrace1
__sys_closesocket_nt:
        .ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_closesocket(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	__sys_closesocket_nt,globl
	.previous
