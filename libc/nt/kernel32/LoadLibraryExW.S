#include "libc/nt/codegen.h"
.imp	kernel32,__imp_LoadLibraryExW,LoadLibraryExW,0

#ifdef __x86_64__
	.text.windows
LoadLibraryEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_LoadLibraryExW(%rip),%rax
	jmp	__sysv2nt
	.endfn	LoadLibraryEx,globl
	.previous
#endif
