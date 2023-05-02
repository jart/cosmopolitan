#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetExitCodeThread,GetExitCodeThread,0

#ifdef __x86_64__
	.text.windows
GetExitCodeThread:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetExitCodeThread(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetExitCodeThread,globl
	.previous
#endif
