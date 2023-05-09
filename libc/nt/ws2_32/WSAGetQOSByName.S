#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSAGetQOSByName,WSAGetQOSByName,69

	.text.windows
WSAGetQOSByName:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSAGetQOSByName(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	WSAGetQOSByName,globl
	.previous
