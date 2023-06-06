#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
eloop:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	ELOOP(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ELOOP
	ldrsw	x0,[x1,#:lo12:ELOOP]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eloop,globl,hidden
