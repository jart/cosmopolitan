#include "libc/nt/codegen.h"
.imp	user32,__imp_FillRect,FillRect,1780

#ifdef __x86_64__
	.text.windows
FillRect:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_FillRect(%rip),%rax
	jmp	__sysv2nt
	.endfn	FillRect,globl
	.previous
#endif
