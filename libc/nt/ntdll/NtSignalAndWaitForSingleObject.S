#include "libc/nt/ntdllimport.h"
.ntimp	NtSignalAndWaitForSingleObject,NtSignalAndWaitForSingleObject

	.text.windows
NtSignalAndWaitForSingleObject:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtSignalAndWaitForSingleObject(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtSignalAndWaitForSingleObject,globl
	.previous
