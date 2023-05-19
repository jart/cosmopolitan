#include "libc/macros.internal.h"
.text.unlikely

edom:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EDOM(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EDOM
	ldrsw	x0,[x1,#:lo12:EDOM]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	edom,globl,hidden
