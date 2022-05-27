#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

eagain:	.leafprologue
	mov	EAGAIN(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eagain,globl,hidden
