#include "libc/nt/ntdllimport.h"
.ntimp	NtQueueApcThread,NtQueueApcThread

	.text.windows
	.ftrace1
NtQueueApcThread:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_NtQueueApcThread(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtQueueApcThread,globl
	.previous
