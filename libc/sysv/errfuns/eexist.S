#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
eexist:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EEXIST(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EEXIST
	ldrsw	x0,[x1,#:lo12:EEXIST]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eexist,globl,hidden
