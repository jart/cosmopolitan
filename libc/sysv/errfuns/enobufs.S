#include "libc/macros.internal.h"
.text.unlikely

enobufs:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ENOBUFS(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENOBUFS
	ldrsw	x0,[x1,#:lo12:ENOBUFS]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enobufs,globl,hidden
