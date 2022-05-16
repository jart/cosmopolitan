#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

etxtbsy:
	.leafprologue
	mov	ETXTBSY(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	etxtbsy,globl,hidden
