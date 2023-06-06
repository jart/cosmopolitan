#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
eprotonosupport:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EPROTONOSUPPORT(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EPROTONOSUPPORT
	ldrsw	x0,[x1,#:lo12:EPROTONOSUPPORT]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eprotonosupport,globl,hidden
