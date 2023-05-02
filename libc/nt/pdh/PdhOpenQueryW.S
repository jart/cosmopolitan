#include "libc/nt/codegen.h"
.imp	pdh,__imp_PdhOpenQueryW,PdhOpenQueryW,0

#ifdef __x86_64__
	.text.windows
PdhOpenQuery:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_PdhOpenQueryW(%rip),%rax
	jmp	__sysv2nt
	.endfn	PdhOpenQuery,globl
	.previous
#endif
