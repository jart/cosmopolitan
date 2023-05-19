#include "libc/macros.internal.h"
.text.unlikely

efbig:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EFBIG(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EFBIG
	ldrsw	x0,[x1,#:lo12:EFBIG]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	efbig,globl,hidden
