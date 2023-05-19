#include "libc/macros.internal.h"
.text.unlikely

enetdown:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ENETDOWN(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENETDOWN
	ldrsw	x0,[x1,#:lo12:ENETDOWN]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enetdown,globl,hidden
