#include "libc/nt/codegen.h"
.imp	user32,__imp_InvalidateRect,InvalidateRect,2048

#ifdef __x86_64__
	.text.windows
InvalidateRect:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_InvalidateRect(%rip),%rax
	jmp	__sysv2nt
	.endfn	InvalidateRect,globl
	.previous
#endif
