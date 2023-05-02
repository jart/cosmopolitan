#include "libc/nt/codegen.h"
.imp	user32,__imp_EndPaint,EndPaint,1752

#ifdef __x86_64__
	.text.windows
EndPaint:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_EndPaint(%rip),%rax
	jmp	__sysv2nt
	.endfn	EndPaint,globl
	.previous
#endif
