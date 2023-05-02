#include "libc/nt/codegen.h"
.imp	kernel32,__imp_CreateWaitableTimerExW,CreateWaitableTimerExW,0

#ifdef __x86_64__
	.text.windows
CreateWaitableTimerEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateWaitableTimerExW(%rip),%rax
	jmp	__sysv2nt
	.endfn	CreateWaitableTimerEx,globl
	.previous
#endif
