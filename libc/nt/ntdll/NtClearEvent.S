#include "libc/nt/ntdllimport.h"
.ntimp	NtClearEvent,NtClearEvent

	.text.windows
NtClearEvent:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_NtClearEvent(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	NtClearEvent,globl
	.previous
