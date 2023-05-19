#include "libc/macros.internal.h"
.text.unlikely

elnrng:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ELNRNG(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ELNRNG
	ldrsw	x0,[x1,#:lo12:ELNRNG]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	elnrng,globl,hidden
