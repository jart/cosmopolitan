#include "libc/nt/ntdllimport.h"
.ntimp	ZwAreMappedFilesTheSame,ZwAreMappedFilesTheSame

#ifdef __x86_64__
	.text.windows
ZwAreMappedFilesTheSame:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_ZwAreMappedFilesTheSame(%rip),%rax
	jmp	__sysv2nt
	.endfn	ZwAreMappedFilesTheSame,globl
	.previous
#endif
