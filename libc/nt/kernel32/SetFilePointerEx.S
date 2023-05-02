#include "libc/nt/codegen.h"
.imp	kernel32,__imp_SetFilePointerEx,SetFilePointerEx,0

#ifdef __x86_64__
	.text.windows
SetFilePointerEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetFilePointerEx(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetFilePointerEx,globl
	.previous
#endif
