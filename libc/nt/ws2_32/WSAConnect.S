#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSAConnect,WSAConnect

	.text.windows
	.ftrace1
WSAConnect:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_WSAConnect(%rip),%rax
	jmp	__sysv2nt8
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	WSAConnect,globl
	.previous
