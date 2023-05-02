#include "libc/nt/codegen.h"
.imp	user32,__imp_KillTimer,KillTimer,2093

#ifdef __x86_64__
	.text.windows
KillTimer:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_KillTimer(%rip),%rax
	jmp	__sysv2nt
	.endfn	KillTimer,globl
	.previous
#endif
