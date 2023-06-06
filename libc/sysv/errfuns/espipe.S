#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
espipe:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	ESPIPE(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ESPIPE
	ldrsw	x0,[x1,#:lo12:ESPIPE]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	espipe,globl,hidden
