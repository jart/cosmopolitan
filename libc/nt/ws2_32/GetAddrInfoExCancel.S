#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_GetAddrInfoExCancel,GetAddrInfoExCancel,29

#ifdef __x86_64__
	.text.windows
GetAddrInfoExCancel:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetAddrInfoExCancel(%rip)
	leave
	ret
	.endfn	GetAddrInfoExCancel,globl
	.previous
#endif
