#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

eisdir:	.leafprologue
	mov	EISDIR(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eisdir,globl,hidden
