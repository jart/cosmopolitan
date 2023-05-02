#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSAGetQOSByName,WSAGetQOSByName,69

#ifdef __x86_64__
	.text.windows
WSAGetQOSByName:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSAGetQOSByName(%rip),%rax
	jmp	__sysv2nt
	.endfn	WSAGetQOSByName,globl
	.previous
#endif
