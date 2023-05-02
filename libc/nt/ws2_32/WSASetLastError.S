#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSASetLastError,WSASetLastError,112

#ifdef __x86_64__
	.text.windows
WSASetLastError:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_WSASetLastError(%rip)
	leave
	ret
	.endfn	WSASetLastError,globl
	.previous
#endif
