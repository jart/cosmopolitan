#include "libc/nt/ntdllimport.h"
.ntimp	NtQuerySystemTime,NtQuerySystemTime

#ifdef __x86_64__
	.text.windows
NtQuerySystemTime:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_NtQuerySystemTime(%rip)
	leave
	ret
	.endfn	NtQuerySystemTime,globl
	.previous
#endif
