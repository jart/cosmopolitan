#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

enodata:
	.leafprologue
	mov	ENODATA(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enodata,globl,hidden
