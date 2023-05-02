#include "libc/nt/codegen.h"
.imp	kernel32,__imp_SetFilePointer,SetFilePointer,0

#ifdef __x86_64__
	.text.windows
SetFilePointer:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetFilePointer(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetFilePointer,globl
	.previous
#endif
