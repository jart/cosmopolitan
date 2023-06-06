#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
enotblk:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	ENOTBLK(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENOTBLK
	ldrsw	x0,[x1,#:lo12:ENOTBLK]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enotblk,globl,hidden
