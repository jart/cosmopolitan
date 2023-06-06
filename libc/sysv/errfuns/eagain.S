#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
eagain:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EAGAIN(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EAGAIN
	ldrsw	x0,[x1,#:lo12:EAGAIN]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eagain,globl,hidden
