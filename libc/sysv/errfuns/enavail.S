#include "libc/macros.internal.h"
.text.unlikely

enavail:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ENAVAIL(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENAVAIL
	ldrsw	x0,[x1,#:lo12:ENAVAIL]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enavail,globl,hidden
