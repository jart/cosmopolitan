#include "libc/nt/codegen.h"
.imp	kernel32,__imp_ReadFileEx,ReadFileEx,0

#ifdef __x86_64__
	.text.windows
ReadFileEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_ReadFileEx(%rip),%rax
	jmp	__sysv2nt6
	.endfn	ReadFileEx,globl
	.previous
#endif
