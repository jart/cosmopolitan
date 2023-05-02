#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSALookupServiceEnd,WSALookupServiceEnd,82

#ifdef __x86_64__
	.text.windows
WSALookupServiceEnd:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_WSALookupServiceEnd(%rip)
	leave
	ret
	.endfn	WSALookupServiceEnd,globl
	.previous
#endif
