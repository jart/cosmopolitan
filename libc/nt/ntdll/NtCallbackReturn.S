#include "libc/nt/ntdllimport.h"
.ntimp	NtCallbackReturn,NtCallbackReturn

#ifdef __x86_64__
	.text.windows
NtCallbackReturn:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtCallbackReturn(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtCallbackReturn,globl
	.previous
#endif
