#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
ekeyrejected:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EKEYREJECTED(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EKEYREJECTED
	ldrsw	x0,[x1,#:lo12:EKEYREJECTED]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	ekeyrejected,globl,hidden
