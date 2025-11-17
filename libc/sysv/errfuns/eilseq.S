#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
eilseq:
	.ftrace2
#ifdef __x86_64__
	push	$EILSEQ
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#EILSEQ
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	eilseq,globl,hidden
