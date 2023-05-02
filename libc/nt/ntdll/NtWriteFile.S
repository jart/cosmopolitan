#include "libc/nt/ntdllimport.h"
.ntimp	NtWriteFile,NtWriteFile

#ifdef __x86_64__
	.text.windows
NtWriteFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtWriteFile(%rip),%rax
	jmp	__sysv2nt10
	.endfn	NtWriteFile,globl
	.previous
#endif
