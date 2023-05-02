#include "libc/nt/ntdllimport.h"
.ntimp	NtCreateTimer,NtCreateTimer

#ifdef __x86_64__
	.text.windows
NtCreateTimer:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtCreateTimer(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtCreateTimer,globl
	.previous
#endif
