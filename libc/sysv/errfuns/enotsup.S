#include "libc/macros.internal.h"
.text.unlikely

enotsup:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ENOTSUP(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENOTSUP
	ldrsw	x0,[x1,#:lo12:ENOTSUP]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enotsup,globl,hidden
