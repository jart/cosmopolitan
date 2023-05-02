#include "libc/nt/codegen.h"
.imp	kernel32,__imp_QueryPerformanceFrequency,QueryPerformanceFrequency,1099

#ifdef __x86_64__
	.text.windows
QueryPerformanceFrequency:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_QueryPerformanceFrequency(%rip)
	leave
	ret
	.endfn	QueryPerformanceFrequency,globl
	.previous
#endif
