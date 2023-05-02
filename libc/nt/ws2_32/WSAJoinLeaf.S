#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSAJoinLeaf,WSAJoinLeaf,79

#ifdef __x86_64__
	.text.windows
WSAJoinLeaf:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSAJoinLeaf(%rip),%rax
	jmp	__sysv2nt8
	.endfn	WSAJoinLeaf,globl
	.previous
#endif
