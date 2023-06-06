#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
eacces:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EACCES(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EACCES
	ldrsw	x0,[x1,#:lo12:EACCES]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eacces,globl,hidden
