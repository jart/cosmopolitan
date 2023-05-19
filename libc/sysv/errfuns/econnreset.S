#include "libc/macros.internal.h"
.text.unlikely

econnreset:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ECONNRESET(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ECONNRESET
	ldrsw	x0,[x1,#:lo12:ECONNRESET]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	econnreset,globl,hidden
