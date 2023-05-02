#include "libc/nt/codegen.h"
.imp	kernel32,__imp_CopyFileW,CopyFileW,0

#ifdef __x86_64__
	.text.windows
CopyFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CopyFileW(%rip),%rax
	jmp	__sysv2nt
	.endfn	CopyFile,globl
	.previous
#endif
