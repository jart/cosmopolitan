#include "libc/nt/codegen.h"
.imp	user32,__imp_EndPaint,EndPaint,1752

	.text.windows
EndPaint:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_EndPaint(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	EndPaint,globl
	.previous
