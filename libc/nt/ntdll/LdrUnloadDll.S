#include "libc/nt/ntdllimport.h"
.ntimp	LdrUnloadDll,LdrUnloadDll

#ifdef __x86_64__
	.text.windows
LdrUnloadDll:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_LdrUnloadDll(%rip)
	leave
	ret
	.endfn	LdrUnloadDll,globl
	.previous
#endif
