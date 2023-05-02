#include "libc/nt/ntdllimport.h"
.ntimp	NtQueryDirectoryFile,NtQueryDirectoryFile

#ifdef __x86_64__
	.text.windows
NtQueryDirectoryFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtQueryDirectoryFile(%rip),%rax
	jmp	__sysv2nt12
	.endfn	NtQueryDirectoryFile,globl
	.previous
#endif
