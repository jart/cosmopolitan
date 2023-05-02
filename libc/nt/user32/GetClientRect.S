#include "libc/nt/codegen.h"
.imp	user32,__imp_GetClientRect,GetClientRect,1815

#ifdef __x86_64__
	.text.windows
GetClientRect:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetClientRect(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetClientRect,globl
	.previous
#endif
