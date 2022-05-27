#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

ekeyexpired:
	.leafprologue
	mov	EKEYEXPIRED(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	ekeyexpired,globl,hidden
