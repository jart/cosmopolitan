#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
enotempty:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	ENOTEMPTY(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENOTEMPTY
	ldrsw	x0,[x1,#:lo12:ENOTEMPTY]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enotempty,globl,hidden
