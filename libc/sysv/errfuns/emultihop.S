#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
emultihop:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EMULTIHOP(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EMULTIHOP
	ldrsw	x0,[x1,#:lo12:EMULTIHOP]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	emultihop,globl,hidden
