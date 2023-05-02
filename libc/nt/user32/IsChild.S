#include "libc/nt/codegen.h"
.imp	user32,__imp_IsChild,IsChild,2059

#ifdef __x86_64__
	.text.windows
IsChild:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_IsChild(%rip),%rax
	jmp	__sysv2nt
	.endfn	IsChild,globl
	.previous
#endif
