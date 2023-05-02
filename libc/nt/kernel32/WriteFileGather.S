#include "libc/nt/codegen.h"
.imp	kernel32,__imp_WriteFileGather,WriteFileGather,0

#ifdef __x86_64__
	.text.windows
WriteFileGather:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WriteFileGather(%rip),%rax
	jmp	__sysv2nt6
	.endfn	WriteFileGather,globl
	.previous
#endif
