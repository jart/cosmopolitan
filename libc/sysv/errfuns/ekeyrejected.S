#include "libc/macros.internal.h"
.text.unlikely

ekeyrejected:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EKEYREJECTED(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EKEYREJECTED
	ldr	w0,[x1,#:lo12:EKEYREJECTED]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	ekeyrejected,globl,hidden
