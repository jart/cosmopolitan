#include "libc/macros.h"
.text.unlikely

ebadrqc:
	.leafprologue
	.profilable
	mov	EBADRQC(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	ebadrqc,globl,hidden
