#include "libc/nt/codegen.h"
.imp	user32,__imp_BeginPaint,BeginPaint,1521

#ifdef __x86_64__
	.text.windows
BeginPaint:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_BeginPaint(%rip),%rax
	jmp	__sysv2nt
	.endfn	BeginPaint,globl
	.previous
#endif
