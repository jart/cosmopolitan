#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSAGetLastError,WSAGetLastError

	.text.windows
	.ftrace1
WSAGetLastError:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	sub	$32,%rsp
	call	*__imp_WSAGetLastError(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	WSAGetLastError,globl
	.previous
