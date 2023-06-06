#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
enostr:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	ENOSTR(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENOSTR
	ldrsw	x0,[x1,#:lo12:ENOSTR]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enostr,globl,hidden
