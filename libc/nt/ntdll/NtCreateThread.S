#include "libc/nt/ntdllimport.h"
.ntimp	NtCreateThread,NtCreateThread

	.text.windows
NtCreateThread:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtCreateThread(%rip),%rax
	jmp	__sysv2nt8
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtCreateThread,globl
	.previous
