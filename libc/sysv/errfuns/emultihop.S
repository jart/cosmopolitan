#include "libc/macros.h"
.text.unlikely

emultihop:
	.leafprologue
	.profilable
	mov	EMULTIHOP(%rip),%eax
	mov	%eax,errno(%rip)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	emultihop,globl,hidden
