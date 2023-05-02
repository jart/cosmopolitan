#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetCurrentDirectoryW,GetCurrentDirectoryW,0

#ifdef __x86_64__
	.text.windows
GetCurrentDirectory:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetCurrentDirectoryW(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetCurrentDirectory,globl
	.previous
#endif
