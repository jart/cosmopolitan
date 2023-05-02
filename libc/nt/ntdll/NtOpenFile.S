#include "libc/nt/ntdllimport.h"
.ntimp	NtOpenFile,NtOpenFile

#ifdef __x86_64__
	.text.windows
NtOpenFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtOpenFile(%rip),%rax
	jmp	__sysv2nt6
	.endfn	NtOpenFile,globl
	.previous
#endif
