#include "libc/nt/codegen.h"
.imp	gdi32,__imp_SelectObject,SelectObject,1864

#ifdef __x86_64__
	.text.windows
SelectObject:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SelectObject(%rip),%rax
	jmp	__sysv2nt
	.endfn	SelectObject,globl
	.previous
#endif
