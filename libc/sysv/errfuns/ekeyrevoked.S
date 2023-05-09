#include "libc/macros.internal.h"
.text.unlikely

ekeyrevoked:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EKEYREVOKED(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EKEYREVOKED
	ldr	w0,[x1,#:lo12:EKEYREVOKED]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	ekeyrevoked,globl,hidden
