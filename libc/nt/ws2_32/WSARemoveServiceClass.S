#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSARemoveServiceClass,WSARemoveServiceClass,94

	.text.windows
WSARemoveServiceClass:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_WSARemoveServiceClass(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	WSARemoveServiceClass,globl
	.previous
