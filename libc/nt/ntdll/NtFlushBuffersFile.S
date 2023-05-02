#include "libc/nt/ntdllimport.h"
.ntimp	NtFlushBuffersFile,NtFlushBuffersFile

#ifdef __x86_64__
	.text.windows
NtFlushBuffersFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtFlushBuffersFile(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtFlushBuffersFile,globl
	.previous
#endif
