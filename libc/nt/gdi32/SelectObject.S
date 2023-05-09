#include "libc/nt/codegen.h"
.imp	gdi32,__imp_SelectObject,SelectObject,1864

	.text.windows
SelectObject:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SelectObject(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	SelectObject,globl
	.previous
