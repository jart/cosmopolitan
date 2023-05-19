#include "libc/macros.internal.h"
.text.unlikely

ebadmsg:
#ifdef __x86_64__
	.leafprologue
	.profilable
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
