#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
enotconn:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	ENOTCONN(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENOTCONN
	ldrsw	x0,[x1,#:lo12:ENOTCONN]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enotconn,globl,hidden
