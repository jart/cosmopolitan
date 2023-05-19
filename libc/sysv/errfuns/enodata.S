#include "libc/macros.internal.h"
.text.unlikely

enodata:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ENODATA(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENODATA
	ldrsw	x0,[x1,#:lo12:ENODATA]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enodata,globl,hidden
