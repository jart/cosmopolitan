#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

elibbad:
	.leafprologue
	mov	ELIBBAD(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	elibbad,globl,hidden
