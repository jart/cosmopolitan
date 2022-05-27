#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

euclean:
	.leafprologue
	mov	EUCLEAN(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	euclean,globl,hidden
