#include "libc/nt/codegen.h"
.imp	gdi32,__imp_SetTextAlign,SetTextAlign,1910

#ifdef __x86_64__
	.text.windows
SetTextAlign:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetTextAlign(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetTextAlign,globl
	.previous
#endif
