#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
eprototype:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EPROTOTYPE(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EPROTOTYPE
	ldrsw	x0,[x1,#:lo12:EPROTOTYPE]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eprototype,globl,hidden
