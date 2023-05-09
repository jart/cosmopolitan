#include "libc/nt/codegen.h"
.imp	user32,__imp_GetWindowRect,GetWindowRect,1997

	.text.windows
GetWindowRect:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetWindowRect(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	GetWindowRect,globl
	.previous
