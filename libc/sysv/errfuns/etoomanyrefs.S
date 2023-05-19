#include "libc/macros.internal.h"
.text.unlikely

etoomanyrefs:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ETOOMANYREFS(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ETOOMANYREFS
	ldrsw	x0,[x1,#:lo12:ETOOMANYREFS]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	etoomanyrefs,globl,hidden
