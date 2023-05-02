#include "libc/nt/ntdllimport.h"
.ntimp	NtFsControlFile,NtFsControlFile

#ifdef __x86_64__
	.text.windows
NtFsControlFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtFsControlFile(%rip),%rax
	jmp	__sysv2nt10
	.endfn	NtFsControlFile,globl
	.previous
#endif
