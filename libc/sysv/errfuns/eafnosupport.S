#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
eafnosupport:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EAFNOSUPPORT(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EAFNOSUPPORT
	ldrsw	x0,[x1,#:lo12:EAFNOSUPPORT]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eafnosupport,globl,hidden
