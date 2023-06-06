#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
ehwpoison:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EHWPOISON(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EHWPOISON
	ldrsw	x0,[x1,#:lo12:EHWPOISON]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	ehwpoison,globl,hidden
