#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
enfile:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	ENFILE(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENFILE
	ldrsw	x0,[x1,#:lo12:ENFILE]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enfile,globl,hidden
