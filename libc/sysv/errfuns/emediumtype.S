#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
emediumtype:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EMEDIUMTYPE(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EMEDIUMTYPE
	ldrsw	x0,[x1,#:lo12:EMEDIUMTYPE]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	emediumtype,globl,hidden
