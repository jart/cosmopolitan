#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
ebade:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EBADE(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EBADE
	ldrsw	x0,[x1,#:lo12:EBADE]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	ebade,globl,hidden
