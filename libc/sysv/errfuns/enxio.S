#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

enxio:	.leafprologue
	mov	ENXIO(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	enxio,globl,hidden
