#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
enxio:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	ENXIO(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENXIO
	ldrsw	x0,[x1,#:lo12:ENXIO]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enxio,globl,hidden
