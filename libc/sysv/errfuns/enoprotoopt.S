#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

enoprotoopt:
	.leafprologue
	mov	ENOPROTOOPT(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enoprotoopt,globl,hidden
