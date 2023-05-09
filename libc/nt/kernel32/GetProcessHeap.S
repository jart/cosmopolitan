#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetProcessHeap,GetProcessHeap,0

	.text.windows
GetProcessHeap:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_GetProcessHeap(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	GetProcessHeap,globl
	.previous
