#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
eio:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EIO(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EIO
	ldrsw	x0,[x1,#:lo12:EIO]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eio,globl,hidden
