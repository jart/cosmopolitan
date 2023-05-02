#include "libc/nt/codegen.h"
.imp	kernel32,__imp_MoveFileExW,MoveFileExW,0

#ifdef __x86_64__
	.text.windows
__MoveFileEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_MoveFileExW(%rip),%rax
	jmp	__sysv2nt
	.endfn	__MoveFileEx,globl
	.previous
#endif
