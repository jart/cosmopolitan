#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetTempPathA,GetTempPathA,0

#ifdef __x86_64__
	.text.windows
GetTempPathA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetTempPathA(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetTempPathA,globl
	.previous
#endif
