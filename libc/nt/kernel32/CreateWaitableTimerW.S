#include "libc/nt/codegen.h"
.imp	kernel32,__imp_CreateWaitableTimerW,CreateWaitableTimerW,0

#ifdef __x86_64__
	.text.windows
CreateWaitableTimer:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateWaitableTimerW(%rip),%rax
	jmp	__sysv2nt
	.endfn	CreateWaitableTimer,globl
	.previous
#endif
