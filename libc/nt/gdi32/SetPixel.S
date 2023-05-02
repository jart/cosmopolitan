#include "libc/nt/codegen.h"
.imp	gdi32,__imp_SetPixel,SetPixel,1901

#ifdef __x86_64__
	.text.windows
SetPixel:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetPixel(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetPixel,globl
	.previous
#endif
