#include "libc/nt/codegen.h"
.imp	kernel32,__imp_MoveFileW,MoveFileW,1000

#ifdef __x86_64__
	.text.windows
MoveFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_MoveFileW(%rip),%rax
	jmp	__sysv2nt
	.endfn	MoveFile,globl
	.previous
#endif
