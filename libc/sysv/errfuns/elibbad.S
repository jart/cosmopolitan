#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
elibbad:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	ELIBBAD(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ELIBBAD
	ldrsw	x0,[x1,#:lo12:ELIBBAD]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	elibbad,globl,hidden
