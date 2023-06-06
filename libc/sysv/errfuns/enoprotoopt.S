#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
enoprotoopt:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	ENOPROTOOPT(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENOPROTOOPT
	ldrsw	x0,[x1,#:lo12:ENOPROTOOPT]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enoprotoopt,globl,hidden
