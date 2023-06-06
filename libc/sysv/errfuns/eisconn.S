#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
eisconn:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EISCONN(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EISCONN
	ldrsw	x0,[x1,#:lo12:EISCONN]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eisconn,globl,hidden
