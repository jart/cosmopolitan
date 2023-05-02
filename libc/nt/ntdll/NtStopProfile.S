#include "libc/nt/ntdllimport.h"
.ntimp	NtStopProfile,NtStopProfile

#ifdef __x86_64__
	.text.windows
NtStopProfile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_NtStopProfile(%rip)
	leave
	ret
	.endfn	NtStopProfile,globl
	.previous
#endif
