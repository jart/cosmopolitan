#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_FreeAddrInfoW,FreeAddrInfoW,27

#ifdef __x86_64__
	.text.windows
FreeAddrInfo:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_FreeAddrInfoW(%rip)
	leave
	ret
	.endfn	FreeAddrInfo,globl
	.previous
#endif
