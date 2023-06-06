#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
el2nsync:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EL2NSYNC(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EL2NSYNC
	ldrsw	x0,[x1,#:lo12:EL2NSYNC]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	el2nsync,globl,hidden
