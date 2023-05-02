#include "libc/nt/ntdllimport.h"
.ntimp	NtOpenProcess,NtOpenProcess

#ifdef __x86_64__
	.text.windows
NtOpenProcess:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtOpenProcess(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtOpenProcess,globl
	.previous
#endif
