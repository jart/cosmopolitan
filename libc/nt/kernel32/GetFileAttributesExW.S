#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetFileAttributesExW,GetFileAttributesExW,0

#ifdef __x86_64__
	.text.windows
GetFileAttributesEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetFileAttributesExW(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetFileAttributesEx,globl
	.previous
#endif
