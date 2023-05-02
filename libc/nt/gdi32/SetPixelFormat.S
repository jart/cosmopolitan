#include "libc/nt/codegen.h"
.imp	gdi32,__imp_SetPixelFormat,SetPixelFormat,1902

#ifdef __x86_64__
	.text.windows
SetPixelFormat:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetPixelFormat(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetPixelFormat,globl
	.previous
#endif
