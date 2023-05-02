#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetWindowsDirectoryA,GetWindowsDirectoryA,0

#ifdef __x86_64__
	.text.windows
GetWindowsDirectoryA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetWindowsDirectoryA(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetWindowsDirectoryA,globl
	.previous
#endif
