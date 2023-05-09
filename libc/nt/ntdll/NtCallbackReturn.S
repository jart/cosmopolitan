#include "libc/nt/ntdllimport.h"
.ntimp	NtCallbackReturn,NtCallbackReturn

	.text.windows
NtCallbackReturn:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtCallbackReturn(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtCallbackReturn,globl
	.previous
