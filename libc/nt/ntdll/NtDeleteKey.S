#include "libc/nt/ntdllimport.h"
.ntimp	NtDeleteKey,NtDeleteKey

#ifdef __x86_64__
	.text.windows
NtDeleteKey:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_NtDeleteKey(%rip)
	leave
	ret
	.endfn	NtDeleteKey,globl
	.previous
#endif
