#include "libc/nt/ntdllimport.h"
.ntimp	NtFlushKey,NtFlushKey

#ifdef __x86_64__
	.text.windows
NtFlushKey:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_NtFlushKey(%rip)
	leave
	ret
	.endfn	NtFlushKey,globl
	.previous
#endif
