#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
ebadrqc:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EBADRQC(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EBADRQC
	ldrsw	x0,[x1,#:lo12:EBADRQC]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	ebadrqc,globl,hidden
