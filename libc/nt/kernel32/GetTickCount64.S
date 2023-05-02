#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetTickCount64,GetTickCount64,0

#ifdef __x86_64__
	.text.windows
GetTickCount64:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_GetTickCount64(%rip)
	leave
	ret
	.endfn	GetTickCount64,globl
	.previous
#endif
