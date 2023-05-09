#include "libc/macros.internal.h"
.text.unlikely

elibbad:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ELIBBAD(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ELIBBAD
	ldr	w0,[x1,#:lo12:ELIBBAD]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	elibbad,globl,hidden
