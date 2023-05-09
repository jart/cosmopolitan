#include "libc/nt/codegen.h"
.imp	user32,__imp_IsChild,IsChild,2059

	.text.windows
IsChild:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_IsChild(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	IsChild,globl
	.previous
