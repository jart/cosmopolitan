#include "libc/macros.internal.h"
.text.unlikely

eaddrinuse:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EADDRINUSE(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EADDRINUSE
	ldrsw	x0,[x1,#:lo12:EADDRINUSE]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eaddrinuse,globl,hidden
