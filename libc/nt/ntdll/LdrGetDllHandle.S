#include "libc/nt/ntdllimport.h"
.ntimp	LdrGetDllHandle,LdrGetDllHandle

#ifdef __x86_64__
	.text.windows
LdrGetDllHandle:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_LdrGetDllHandle(%rip),%rax
	jmp	__sysv2nt
	.endfn	LdrGetDllHandle,globl
	.previous
#endif
