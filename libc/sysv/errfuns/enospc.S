#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
enospc:
	.ftrace2
#ifdef __x86_64__
	push	$ENOSPC
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#ENOSPC
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	enospc,globl,hidden
