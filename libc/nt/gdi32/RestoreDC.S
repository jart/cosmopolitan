#include "libc/nt/codegen.h"
.imp	gdi32,__imp_RestoreDC,RestoreDC,1808

#ifdef __x86_64__
	.text.windows
RestoreDC:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RestoreDC(%rip),%rax
	jmp	__sysv2nt
	.endfn	RestoreDC,globl
	.previous
#endif
