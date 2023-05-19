#include "libc/macros.internal.h"
.text.unlikely

elibmax:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ELIBMAX(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ELIBMAX
	ldrsw	x0,[x1,#:lo12:ELIBMAX]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	elibmax,globl,hidden
