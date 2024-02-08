#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSASetEvent,WSASetEvent

	.text.windows
	.ftrace1
WSASetEvent:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_WSASetEvent(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	WSASetEvent,globl
	.previous
