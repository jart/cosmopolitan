#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

esrch:	.leafprologue
	mov	ESRCH(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	esrch,globl,hidden
