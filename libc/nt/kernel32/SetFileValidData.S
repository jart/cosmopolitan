#include "libc/nt/codegen.h"
.imp	kernel32,__imp_SetFileValidData,SetFileValidData,0

#ifdef __x86_64__
	.text.windows
SetFileValidData:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetFileValidData(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetFileValidData,globl
	.previous
#endif
