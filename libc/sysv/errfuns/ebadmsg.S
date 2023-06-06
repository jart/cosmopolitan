#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
ebadmsg:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	EBADMSG(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EBADMSG
	ldrsw	x0,[x1,#:lo12:EBADMSG]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	ebadmsg,globl,hidden
