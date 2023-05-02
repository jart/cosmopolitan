#include "libc/nt/ntdllimport.h"
.ntimp	NtSetInformationFile,NtSetInformationFile

#ifdef __x86_64__
	.text.windows
NtSetInformationFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtSetInformationFile(%rip),%rax
	jmp	__sysv2nt6
	.endfn	NtSetInformationFile,globl
	.previous
#endif
