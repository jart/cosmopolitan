#include "libc/macros.internal.h"
.text.unlikely

ebadrqc:
	.leafprologue
	.profilable
	mov	EBADRQC(%rip),%ecx
	jmp	__errfun
	.endfn	ebadrqc,globl,hidden
