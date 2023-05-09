#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetOverlappedResult,GetOverlappedResult,0

	.text.windows
GetOverlappedResult:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetOverlappedResult(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	GetOverlappedResult,globl
	.previous
