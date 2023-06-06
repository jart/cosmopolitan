#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
enetunreach:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	ENETUNREACH(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENETUNREACH
	ldrsw	x0,[x1,#:lo12:ENETUNREACH]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enetunreach,globl,hidden
