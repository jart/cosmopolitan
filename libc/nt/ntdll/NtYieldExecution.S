#include "libc/nt/ntdllimport.h"
.ntimp	NtYieldExecution,NtYieldExecution

#ifdef __x86_64__
	.text.windows
NtYieldExecution:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_NtYieldExecution(%rip)
	leave
	ret
	.endfn	NtYieldExecution,globl
	.previous
#endif
