#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetCurrentThreadId,GetCurrentThreadId,0

#ifdef __x86_64__
	.text.windows
GetCurrentThreadId:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_GetCurrentThreadId(%rip)
	leave
	ret
	.endfn	GetCurrentThreadId,globl
	.previous
#endif
