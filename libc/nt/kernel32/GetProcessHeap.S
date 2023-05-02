#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetProcessHeap,GetProcessHeap,0

#ifdef __x86_64__
	.text.windows
GetProcessHeap:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_GetProcessHeap(%rip)
	leave
	ret
	.endfn	GetProcessHeap,globl
	.previous
#endif
