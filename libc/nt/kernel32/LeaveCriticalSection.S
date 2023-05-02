#include "libc/nt/codegen.h"
.imp	kernel32,__imp_LeaveCriticalSection,LeaveCriticalSection,0

#ifdef __x86_64__
	.text.windows
LeaveCriticalSection:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_LeaveCriticalSection(%rip)
	leave
	ret
	.endfn	LeaveCriticalSection,globl
	.previous
#endif
