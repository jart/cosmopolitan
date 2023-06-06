#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
enotdir:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	ENOTDIR(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENOTDIR
	ldrsw	x0,[x1,#:lo12:ENOTDIR]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enotdir,globl,hidden
