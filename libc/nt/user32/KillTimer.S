#include "libc/nt/codegen.h"
.imp	user32,__imp_KillTimer,KillTimer,2093

	.text.windows
KillTimer:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_KillTimer(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	KillTimer,globl
	.previous
