#include "libc/nt/ntdllimport.h"
.ntimp	NtCreateFile,NtCreateFile

#ifdef __x86_64__
	.text.windows
NtCreateFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtCreateFile(%rip),%rax
	jmp	__sysv2nt12
	.endfn	NtCreateFile,globl
	.previous
#endif
