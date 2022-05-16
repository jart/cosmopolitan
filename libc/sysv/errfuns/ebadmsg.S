#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

ebadmsg:
	.leafprologue
	mov	EBADMSG(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	ebadmsg,globl,hidden
