#include "libc/errno.h"
#include "libc/macros.h"
.text.unlikely

	.ftrace1
emultihop:
	.ftrace2
#ifdef __x86_64__
	push	$EMULTIHOP
	pop	%rdi
	jmp	__errfun
#elif defined(__aarch64__)
	mov	x0,#EMULTIHOP
	b	__errfun
#else
#error "unsupported architecture"
#endif
	.endfn	emultihop,globl,hidden
