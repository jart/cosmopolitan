#include "libc/nt/codegen.h"
.imp	gdi32,__imp_CreateRectRgn,CreateRectRgn,1097

#ifdef __x86_64__
	.text.windows
CreateRectRgn:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateRectRgn(%rip),%rax
	jmp	__sysv2nt
	.endfn	CreateRectRgn,globl
	.previous
#endif
