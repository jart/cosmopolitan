#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
eisdir:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EISDIR(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EISDIR
	ldrsw	x0,[x1,#:lo12:EISDIR]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eisdir,globl,hidden
