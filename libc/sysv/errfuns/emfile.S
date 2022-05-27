#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

emfile:	.leafprologue
	mov	EMFILE(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	emfile,globl,hidden
