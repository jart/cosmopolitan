#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

eilseq:	.leafprologue
	mov	EILSEQ(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eilseq,globl,hidden
