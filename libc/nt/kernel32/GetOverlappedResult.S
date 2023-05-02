#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetOverlappedResult,GetOverlappedResult,0

#ifdef __x86_64__
	.text.windows
GetOverlappedResult:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetOverlappedResult(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetOverlappedResult,globl
	.previous
#endif
