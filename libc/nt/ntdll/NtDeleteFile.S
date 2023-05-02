#include "libc/nt/ntdllimport.h"
.ntimp	NtDeleteFile,NtDeleteFile

#ifdef __x86_64__
	.text.windows
NtDeleteFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_NtDeleteFile(%rip)
	leave
	ret
	.endfn	NtDeleteFile,globl
	.previous
#endif
