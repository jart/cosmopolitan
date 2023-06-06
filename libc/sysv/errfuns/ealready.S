#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
ealready:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EALREADY(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EALREADY
	ldrsw	x0,[x1,#:lo12:EALREADY]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	ealready,globl,hidden
