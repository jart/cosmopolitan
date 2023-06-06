#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
emfile:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EMFILE(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EMFILE
	ldrsw	x0,[x1,#:lo12:EMFILE]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	emfile,globl,hidden
