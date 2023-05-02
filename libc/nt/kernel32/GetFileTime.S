#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetFileTime,GetFileTime,0

#ifdef __x86_64__
	.text.windows
GetFileTime:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetFileTime(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetFileTime,globl
	.previous
#endif
