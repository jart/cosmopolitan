#include "libc/macros.internal.h"
.text.unlikely

el3rst:	.leafprologue
	.profilable
	mov	EL3RST(%rip),%ecx
	jmp	__errfun
	.endfn	el3rst,globl,hidden
