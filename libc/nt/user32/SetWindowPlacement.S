#include "libc/nt/codegen.h"
.imp	user32,__imp_SetWindowPlacement,SetWindowPlacement,2399

	.text.windows
SetWindowPlacement:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetWindowPlacement(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	SetWindowPlacement,globl
	.previous
