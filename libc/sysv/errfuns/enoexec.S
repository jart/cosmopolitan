#include "libc/macros.h"
.text.unlikely

enoexec:
	.leafprologue
	.profilable
	mov	ENOEXEC(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enoexec,globl,hidden
