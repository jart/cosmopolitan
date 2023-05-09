#include "libc/macros.internal.h"
.text.unlikely

enomsg:
#ifdef __x86_64__
	.leafprologue
	.profilable
	mov	ENOMSG(%rip),%ecx
	jmp	__errfun
#elif defined(__aarch64__)
	adrp	x1,ENOMSG
	ldr	w0,[x1,#:lo12:ENOMSG]
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enomsg,globl,hidden
