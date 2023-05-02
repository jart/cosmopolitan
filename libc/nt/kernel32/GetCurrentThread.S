#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetCurrentThread,GetCurrentThread,0

#ifdef __x86_64__
	.text.windows
GetCurrentThread:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_GetCurrentThread(%rip)
	leave
	ret
	.endfn	GetCurrentThread,globl
	.previous
#endif
