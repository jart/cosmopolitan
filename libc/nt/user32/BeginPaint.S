#include "libc/nt/codegen.h"
.imp	user32,__imp_BeginPaint,BeginPaint,1521

	.text.windows
BeginPaint:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_BeginPaint(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	BeginPaint,globl
	.previous
