#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetThreadId,GetThreadId,0

#ifdef __x86_64__
	.text.windows
GetThreadId:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetThreadId(%rip)
	leave
	ret
	.endfn	GetThreadId,globl
	.previous
#endif
