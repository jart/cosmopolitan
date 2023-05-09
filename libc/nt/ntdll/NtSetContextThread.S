#include "libc/nt/ntdllimport.h"
.ntimp	NtSetContextThread,NtSetContextThread

	.text.windows
NtSetContextThread:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtSetContextThread(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtSetContextThread,globl
	.previous
