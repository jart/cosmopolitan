#include "libc/nt/codegen.h"
.imp	gdi32,__imp_SetPixel,SetPixel,1901

	.text.windows
SetPixel:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetPixel(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	SetPixel,globl
	.previous
