#include "libc/macros.internal.h"
.text.unlikely

euclean:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EUCLEAN(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EUCLEAN
	ldr	w0,[x1,#:lo12:EUCLEAN]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	euclean,globl,hidden
