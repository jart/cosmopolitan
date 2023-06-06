#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
ebadf:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EBADF(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EBADF
	ldrsw	x0,[x1,#:lo12:EBADF]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	ebadf,globl,hidden
