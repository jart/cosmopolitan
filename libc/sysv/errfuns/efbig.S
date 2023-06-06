#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
efbig:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EFBIG(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EFBIG
	ldrsw	x0,[x1,#:lo12:EFBIG]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	efbig,globl,hidden
