#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

enopkg:	.leafprologue
	mov	ENOPKG(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enopkg,globl,hidden
