#include "libc/nt/codegen.h"
.imp	kernel32,__imp_CreateFileW,CreateFileW,0

#ifdef __x86_64__
	.text.windows
__CreateFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateFileW(%rip),%rax
	jmp	__sysv2nt8
	.endfn	__CreateFile,globl
	.previous
#endif
