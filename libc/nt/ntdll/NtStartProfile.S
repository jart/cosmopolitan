#include "libc/nt/ntdllimport.h"
.ntimp	NtStartProfile,NtStartProfile

#ifdef __x86_64__
	.text.windows
NtStartProfile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_NtStartProfile(%rip)
	leave
	ret
	.endfn	NtStartProfile,globl
	.previous
#endif
