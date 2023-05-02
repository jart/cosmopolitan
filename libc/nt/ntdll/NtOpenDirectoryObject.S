#include "libc/nt/ntdllimport.h"
.ntimp	NtOpenDirectoryObject,NtOpenDirectoryObject

#ifdef __x86_64__
	.text.windows
NtOpenDirectoryObject:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtOpenDirectoryObject(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtOpenDirectoryObject,globl
	.previous
#endif
