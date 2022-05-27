#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

enolck:	.leafprologue
	mov	ENOLCK(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enolck,globl,hidden
