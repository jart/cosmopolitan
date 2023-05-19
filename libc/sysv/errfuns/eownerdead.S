#include "libc/macros.internal.h"
.text.unlikely

eownerdead:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EOWNERDEAD(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EOWNERDEAD
	ldrsw	x0,[x1,#:lo12:EOWNERDEAD]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eownerdead,globl,hidden
