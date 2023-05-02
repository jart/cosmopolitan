#include "libc/nt/codegen.h"
.imp	user32,__imp_SetParent,SetParent,2357

#ifdef __x86_64__
	.text.windows
SetParent:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetParent(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetParent,globl
	.previous
#endif
