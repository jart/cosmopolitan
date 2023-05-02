#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetSystemDirectoryA,GetSystemDirectoryA,0

#ifdef __x86_64__
	.text.windows
GetSystemDirectoryA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetSystemDirectoryA(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetSystemDirectoryA,globl
	.previous
#endif
