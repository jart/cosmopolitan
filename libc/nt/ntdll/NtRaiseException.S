#include "libc/nt/ntdllimport.h"
.ntimp	NtRaiseException,NtRaiseException

#ifdef __x86_64__
	.text.windows
NtRaiseException:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtRaiseException(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtRaiseException,globl
	.previous
#endif
