#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetSystemDirectoryW,GetSystemDirectoryW,0

#ifdef __x86_64__
	.text.windows
GetSystemDirectory:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetSystemDirectoryW(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetSystemDirectory,globl
	.previous
#endif
