#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSAEnumNameSpaceProvidersExW,WSAEnumNameSpaceProvidersExW

	.text.windows
	.ftrace1
WSAEnumNameSpaceProvidersEx:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_WSAEnumNameSpaceProvidersExW(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	WSAEnumNameSpaceProvidersEx,globl
	.previous
