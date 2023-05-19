#include "libc/macros.internal.h"
.text.unlikely

eshutdown:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ESHUTDOWN(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ESHUTDOWN
	ldrsw	x0,[x1,#:lo12:ESHUTDOWN]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eshutdown,globl,hidden
