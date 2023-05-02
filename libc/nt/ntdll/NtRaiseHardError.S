#include "libc/nt/ntdllimport.h"
.ntimp	NtRaiseHardError,NtRaiseHardError

#ifdef __x86_64__
	.text.windows
NtRaiseHardError:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtRaiseHardError(%rip),%rax
	jmp	__sysv2nt6
	.endfn	NtRaiseHardError,globl
	.previous
#endif
