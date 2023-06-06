#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
echild:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	ECHILD(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ECHILD
	ldrsw	x0,[x1,#:lo12:ECHILD]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	echild,globl,hidden
