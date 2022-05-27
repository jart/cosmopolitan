#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

ebade:	.leafprologue
	mov	EBADE(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	ebade,globl,hidden
