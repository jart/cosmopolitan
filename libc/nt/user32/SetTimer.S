#include "libc/nt/codegen.h"
.imp	user32,__imp_SetTimer,SetTimer,2384

#ifdef __x86_64__
	.text.windows
SetTimer:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetTimer(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetTimer,globl
	.previous
#endif
