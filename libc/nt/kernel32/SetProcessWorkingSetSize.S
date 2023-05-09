#include "libc/nt/codegen.h"
.imp	kernel32,__imp_SetProcessWorkingSetSize,SetProcessWorkingSetSize,1356

	.text.windows
SetProcessWorkingSetSize:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetProcessWorkingSetSize(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	SetProcessWorkingSetSize,globl
	.previous
