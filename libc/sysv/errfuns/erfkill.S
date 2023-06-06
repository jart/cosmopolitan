#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
erfkill:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	ERFKILL(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ERFKILL
	ldrsw	x0,[x1,#:lo12:ERFKILL]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	erfkill,globl,hidden
