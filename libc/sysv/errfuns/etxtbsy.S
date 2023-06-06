#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
etxtbsy:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	ETXTBSY(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ETXTBSY
	ldrsw	x0,[x1,#:lo12:ETXTBSY]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	etxtbsy,globl,hidden
