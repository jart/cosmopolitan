#include "libc/nt/codegen.h"
.imp	kernel32,__imp_SetProcessWorkingSetSize,SetProcessWorkingSetSize,1356

#ifdef __x86_64__
	.text.windows
SetProcessWorkingSetSize:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetProcessWorkingSetSize(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetProcessWorkingSetSize,globl
	.previous
#endif
