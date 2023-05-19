#include "libc/macros.internal.h"
.text.unlikely

eacces:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EACCES(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EACCES
	ldrsw	x0,[x1,#:lo12:EACCES]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eacces,globl,hidden
