#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

elnrng:	.leafprologue
	mov	ELNRNG(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	elnrng,globl,hidden
