#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
esocktnosupport:
	.ftrace2
#ifdef __x86_64__
	push	$ESOCKTNOSUPPORT
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#ESOCKTNOSUPPORT
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	esocktnosupport,globl,hidden
