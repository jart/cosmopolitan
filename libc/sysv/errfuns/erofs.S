#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

erofs:	.leafprologue
	mov	EROFS(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	erofs,globl,hidden
