#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetWindowsDirectoryW,GetWindowsDirectoryW,0

#ifdef __x86_64__
	.text.windows
GetWindowsDirectory:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetWindowsDirectoryW(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetWindowsDirectory,globl
	.previous
#endif
