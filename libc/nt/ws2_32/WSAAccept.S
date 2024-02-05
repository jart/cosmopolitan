#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSAAccept,WSAAccept

	.text.windows
	.ftrace1
WSAAccept:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_WSAAccept(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	WSAAccept,globl
	.previous
