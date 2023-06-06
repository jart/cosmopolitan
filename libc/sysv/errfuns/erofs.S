#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
erofs:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EROFS(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EROFS
	ldrsw	x0,[x1,#:lo12:EROFS]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	erofs,globl,hidden
