#include "libc/macros.internal.h"
.text.unlikely

eexist:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EEXIST(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EEXIST
	ldrsw	x0,[x1,#:lo12:EEXIST]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eexist,globl,hidden
