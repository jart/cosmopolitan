#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
enomem:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	ENOMEM(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENOMEM
	ldrsw	x0,[x1,#:lo12:ENOMEM]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enomem,globl,hidden
