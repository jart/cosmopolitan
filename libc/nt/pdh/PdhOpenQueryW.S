#include "libc/nt/codegen.h"
.imp	pdh,__imp_PdhOpenQueryW,PdhOpenQueryW,0

	.text.windows
PdhOpenQuery:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_PdhOpenQueryW(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	PdhOpenQuery,globl
	.previous
