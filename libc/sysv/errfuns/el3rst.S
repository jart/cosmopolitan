#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
el3rst:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EL3RST(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EL3RST
	ldrsw	x0,[x1,#:lo12:EL3RST]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	el3rst,globl,hidden
