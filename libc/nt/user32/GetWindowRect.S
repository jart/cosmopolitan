#include "libc/nt/codegen.h"
.imp	user32,__imp_GetWindowRect,GetWindowRect,1997

#ifdef __x86_64__
	.text.windows
GetWindowRect:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetWindowRect(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetWindowRect,globl
	.previous
#endif
