#include "libc/macros.internal.h"
.text.unlikely

emultihop:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	EMULTIHOP(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,EMULTIHOP
	ldr	w0,[x1,#:lo12:EMULTIHOP]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	emultihop,globl,hidden
