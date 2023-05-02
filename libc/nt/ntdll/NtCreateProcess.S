#include "libc/nt/ntdllimport.h"
.ntimp	NtCreateProcess,NtCreateProcess

#ifdef __x86_64__
	.text.windows
NtCreateProcess:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtCreateProcess(%rip),%rax
	jmp	__sysv2nt8
	.endfn	NtCreateProcess,globl
	.previous
#endif
