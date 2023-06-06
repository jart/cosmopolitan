#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
elnrng:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	ELNRNG(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ELNRNG
	ldrsw	x0,[x1,#:lo12:ELNRNG]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	elnrng,globl,hidden
