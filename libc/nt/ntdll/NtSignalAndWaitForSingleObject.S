#include "libc/nt/ntdllimport.h"
.ntimp	NtSignalAndWaitForSingleObject,NtSignalAndWaitForSingleObject

#ifdef __x86_64__
	.text.windows
NtSignalAndWaitForSingleObject:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtSignalAndWaitForSingleObject(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtSignalAndWaitForSingleObject,globl
	.previous
#endif
