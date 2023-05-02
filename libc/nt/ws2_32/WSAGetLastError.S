#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSAGetLastError,WSAGetLastError,111

#ifdef __x86_64__
	.text.windows
WSAGetLastError:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_WSAGetLastError(%rip)
	leave
	ret
	.endfn	WSAGetLastError,globl
	.previous
#endif
