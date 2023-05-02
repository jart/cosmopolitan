#include "libc/nt/codegen.h"
.imp	user32,__imp_AdjustWindowRect,AdjustWindowRect,1507

#ifdef __x86_64__
	.text.windows
AdjustWindowRect:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_AdjustWindowRect(%rip),%rax
	jmp	__sysv2nt
	.endfn	AdjustWindowRect,globl
	.previous
#endif
