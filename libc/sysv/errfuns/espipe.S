#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

espipe:	.leafprologue
	mov	ESPIPE(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	espipe,globl,hidden
