#include "libc/macros.internal.h"
.text.unlikely

e2big:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	E2BIG(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,E2BIG
	ldrsw	x0,[x1,#:lo12:E2BIG]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	e2big,globl,hidden
