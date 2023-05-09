#include "libc/macros.internal.h"
.text.unlikely

estale:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ESTALE(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ESTALE
	ldr	w0,[x1,#:lo12:ESTALE]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	estale,globl,hidden
