#include "libc/nt/codegen.h"
.imp	kernel32,__imp_SetWaitableTimer,SetWaitableTimer,0

	.text.windows
SetWaitableTimer:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetWaitableTimer(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	SetWaitableTimer,globl
	.previous
