#include "libc/macros.h"
.text.unlikely

eisconn:
	.leafprologue
	.profilable
	mov	EISCONN(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eisconn,globl,hidden
