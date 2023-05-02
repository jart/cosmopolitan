#include "libc/nt/ntdllimport.h"
.ntimp	NtReadFile,NtReadFile

#ifdef __x86_64__
	.text.windows
NtReadFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtReadFile(%rip),%rax
	jmp	__sysv2nt10
	.endfn	NtReadFile,globl
	.previous
#endif
