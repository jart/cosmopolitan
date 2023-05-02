#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetFileSize,GetFileSize,0

#ifdef __x86_64__
	.text.windows
GetFileSize:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetFileSize(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetFileSize,globl
	.previous
#endif
