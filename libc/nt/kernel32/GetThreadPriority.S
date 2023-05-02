#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetThreadPriority,GetThreadPriority,0

#ifdef __x86_64__
	.text.windows
GetThreadPriority:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetThreadPriority(%rip)
	leave
	ret
	.endfn	GetThreadPriority,globl
	.previous
#endif
