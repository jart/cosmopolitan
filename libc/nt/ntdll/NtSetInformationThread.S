#include "libc/nt/ntdllimport.h"
.ntimp	NtSetInformationThread,NtSetInformationThread

	.text.windows
NtSetInformationThread:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtSetInformationThread(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtSetInformationThread,globl
	.previous
