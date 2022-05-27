#include "libc/macros.internal.h"
.text.unlikely

emultihop:
	.leafprologue
	.profilable
	mov	EMULTIHOP(%rip),%ecx
	jmp	__errfun
	.endfn	emultihop,globl,hidden
