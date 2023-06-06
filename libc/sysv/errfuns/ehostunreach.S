#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
ehostunreach:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EHOSTUNREACH(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EHOSTUNREACH
	ldrsw	x0,[x1,#:lo12:EHOSTUNREACH]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	ehostunreach,globl,hidden
