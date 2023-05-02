#include "libc/nt/ntdllimport.h"
.ntimp	NtFlushInstructionCache,NtFlushInstructionCache

#ifdef __x86_64__
	.text.windows
NtFlushInstructionCache:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtFlushInstructionCache(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtFlushInstructionCache,globl
	.previous
#endif
