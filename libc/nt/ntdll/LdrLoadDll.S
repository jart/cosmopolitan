#include "libc/nt/ntdllimport.h"
.ntimp	LdrLoadDll,LdrLoadDll

#ifdef __x86_64__
	.text.windows
LdrLoadDll:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_LdrLoadDll(%rip),%rax
	jmp	__sysv2nt
	.endfn	LdrLoadDll,globl
	.previous
#endif
