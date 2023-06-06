#include "libc/macros.internal.h"
.text.unlikely

	.ftrace1
enomsg:
	.ftrace2
#ifdef __x86_64__
	.leafprologue
	mov	ENOMSG(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENOMSG
	ldrsw	x0,[x1,#:lo12:ENOMSG]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enomsg,globl,hidden
