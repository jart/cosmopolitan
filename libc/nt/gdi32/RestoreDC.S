#include "libc/nt/codegen.h"
.imp	gdi32,__imp_RestoreDC,RestoreDC,1808

	.text.windows
RestoreDC:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RestoreDC(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	RestoreDC,globl
	.previous
