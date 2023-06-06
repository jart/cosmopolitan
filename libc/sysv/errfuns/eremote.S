#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
eremote:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EREMOTE(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EREMOTE
	ldrsw	x0,[x1,#:lo12:EREMOTE]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eremote,globl,hidden
