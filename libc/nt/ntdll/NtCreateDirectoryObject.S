#include "libc/nt/ntdllimport.h"
.ntimp	NtCreateDirectoryObject,NtCreateDirectoryObject

#ifdef __x86_64__
	.text.windows
NtCreateDirectoryObject:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtCreateDirectoryObject(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtCreateDirectoryObject,globl
	.previous
#endif
