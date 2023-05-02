#include "libc/nt/ntdllimport.h"
.ntimp	NtQueryInformationFile,NtQueryInformationFile

#ifdef __x86_64__
	.text.windows
NtQueryInformationFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtQueryInformationFile(%rip),%rax
	jmp	__sysv2nt6
	.endfn	NtQueryInformationFile,globl
	.previous
#endif
