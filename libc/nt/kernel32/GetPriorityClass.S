#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetPriorityClass,GetPriorityClass,0

#ifdef __x86_64__
	.text.windows
GetPriorityClass:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetPriorityClass(%rip)
	leave
	ret
	.endfn	GetPriorityClass,globl
	.previous
#endif
