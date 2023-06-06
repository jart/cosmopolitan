#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
elibscn:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	ELIBSCN(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ELIBSCN
	ldrsw	x0,[x1,#:lo12:ELIBSCN]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	elibscn,globl,hidden
