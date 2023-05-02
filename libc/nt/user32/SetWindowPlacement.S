#include "libc/nt/codegen.h"
.imp	user32,__imp_SetWindowPlacement,SetWindowPlacement,2399

#ifdef __x86_64__
	.text.windows
SetWindowPlacement:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetWindowPlacement(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetWindowPlacement,globl
	.previous
#endif
