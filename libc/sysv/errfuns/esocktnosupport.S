#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
esocktnosupport:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	ESOCKTNOSUPPORT(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ESOCKTNOSUPPORT
	ldrsw	x0,[x1,#:lo12:ESOCKTNOSUPPORT]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	esocktnosupport,globl,hidden
