#include "libc/nt/codegen.h"
.imp	kernel32,__imp_SetFileAttributesW,SetFileAttributesW,0

#ifdef __x86_64__
	.text.windows
SetFileAttributes:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetFileAttributesW(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetFileAttributes,globl
	.previous
#endif
