#include "libc/macros.internal.h"
.text.unlikely

etime:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ETIME(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ETIME
	ldrsw	x0,[x1,#:lo12:ETIME]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	etime,globl,hidden
