#include "libc/nt/codegen.h"
.imp	gdi32,__imp_SetTextColor,SetTextColor,1912

#ifdef __x86_64__
	.text.windows
SetTextColor:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetTextColor(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetTextColor,globl
	.previous
#endif
