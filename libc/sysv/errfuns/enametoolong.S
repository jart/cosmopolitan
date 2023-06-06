#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
enametoolong:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	ENAMETOOLONG(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENAMETOOLONG
	ldrsw	x0,[x1,#:lo12:ENAMETOOLONG]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enametoolong,globl,hidden
