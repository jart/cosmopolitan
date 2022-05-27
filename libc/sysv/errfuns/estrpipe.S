#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

estrpipe:
	.leafprologue
	mov	ESTRPIPE(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	estrpipe,globl,hidden
