#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
estrpipe:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	ESTRPIPE(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ESTRPIPE
	ldrsw	x0,[x1,#:lo12:ESTRPIPE]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	estrpipe,globl,hidden
