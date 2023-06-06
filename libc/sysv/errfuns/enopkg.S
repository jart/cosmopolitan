#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
enopkg:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	ENOPKG(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENOPKG
	ldrsw	x0,[x1,#:lo12:ENOPKG]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enopkg,globl,hidden
