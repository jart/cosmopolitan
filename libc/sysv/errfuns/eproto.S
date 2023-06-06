#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
eproto:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EPROTO(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EPROTO
	ldrsw	x0,[x1,#:lo12:EPROTO]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eproto,globl,hidden
