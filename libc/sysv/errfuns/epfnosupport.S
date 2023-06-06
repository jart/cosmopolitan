#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
epfnosupport:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EPFNOSUPPORT(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EPFNOSUPPORT
	ldrsw	x0,[x1,#:lo12:EPFNOSUPPORT]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	epfnosupport,globl,hidden
