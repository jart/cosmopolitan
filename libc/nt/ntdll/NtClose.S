#include "libc/nt/ntdllimport.h"
.ntimp	NtClose,NtClose

#ifdef __x86_64__
	.text.windows
NtClose:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_NtClose(%rip)
	leave
	ret
	.endfn	NtClose,globl
	.previous
#endif
