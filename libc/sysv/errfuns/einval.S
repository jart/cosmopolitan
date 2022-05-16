#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

einval:	.leafprologue
	mov	EINVAL(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	einval,globl,hidden
