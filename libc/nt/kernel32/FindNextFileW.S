#include "libc/nt/codegen.h"
.imp	kernel32,__imp_FindNextFileW,FindNextFileW,0

#ifdef __x86_64__
	.text.windows
__FindNextFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_FindNextFileW(%rip),%rax
	jmp	__sysv2nt
	.endfn	__FindNextFile,globl
	.previous
#endif
