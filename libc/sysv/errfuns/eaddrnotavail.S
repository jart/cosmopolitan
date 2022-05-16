#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

eaddrnotavail:
	.leafprologue
	mov	EADDRNOTAVAIL(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eaddrnotavail,globl,hidden
