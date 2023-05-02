#include "libc/nt/ntdllimport.h"
.ntimp	NtQueryVolumeInformationFile,NtQueryVolumeInformationFile

#ifdef __x86_64__
	.text.windows
NtQueryVolumeInformationFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtQueryVolumeInformationFile(%rip),%rax
	jmp	__sysv2nt6
	.endfn	NtQueryVolumeInformationFile,globl
	.previous
#endif
