#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
enosys:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	ENOSYS(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENOSYS
	ldrsw	x0,[x1,#:lo12:ENOSYS]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enosys,globl,hidden
