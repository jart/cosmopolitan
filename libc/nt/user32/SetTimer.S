#include "libc/nt/codegen.h"
.imp	user32,__imp_SetTimer,SetTimer,2384

	.text.windows
SetTimer:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetTimer(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	SetTimer,globl
	.previous
