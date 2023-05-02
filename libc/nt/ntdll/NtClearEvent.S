#include "libc/nt/ntdllimport.h"
.ntimp	NtClearEvent,NtClearEvent

#ifdef __x86_64__
	.text.windows
NtClearEvent:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_NtClearEvent(%rip)
	leave
	ret
	.endfn	NtClearEvent,globl
	.previous
#endif
