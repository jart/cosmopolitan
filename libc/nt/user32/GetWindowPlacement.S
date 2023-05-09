#include "libc/nt/codegen.h"
.imp	user32,__imp_GetWindowPlacement,GetWindowPlacement,1995

	.text.windows
GetWindowPlacement:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetWindowPlacement(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	GetWindowPlacement,globl
	.previous
