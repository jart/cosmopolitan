#include "libc/macros.internal.h"
.text.unlikely

ecanceled:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ECANCELED(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ECANCELED
	ldr	w0,[x1,#:lo12:ECANCELED]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	ecanceled,globl,hidden
