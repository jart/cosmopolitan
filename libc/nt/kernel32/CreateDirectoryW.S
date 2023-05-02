#include "libc/nt/codegen.h"
.imp	kernel32,__imp_CreateDirectoryW,CreateDirectoryW,0

#ifdef __x86_64__
	.text.windows
__CreateDirectory:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateDirectoryW(%rip),%rax
	jmp	__sysv2nt
	.endfn	__CreateDirectory,globl
	.previous
#endif
