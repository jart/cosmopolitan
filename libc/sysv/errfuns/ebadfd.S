#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
ebadfd:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EBADFD(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EBADFD
	ldrsw	x0,[x1,#:lo12:EBADFD]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	ebadfd,globl,hidden
