#include "libc/nt/codegen.h"
.imp	user32,__imp_InvalidateRect,InvalidateRect,2048

	.text.windows
InvalidateRect:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_InvalidateRect(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	InvalidateRect,globl
	.previous
