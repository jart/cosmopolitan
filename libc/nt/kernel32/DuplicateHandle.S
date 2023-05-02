#include "libc/nt/codegen.h"
.imp	kernel32,__imp_DuplicateHandle,DuplicateHandle,0

#ifdef __x86_64__
	.text.windows
DuplicateHandle:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_DuplicateHandle(%rip),%rax
	jmp	__sysv2nt8
	.endfn	DuplicateHandle,globl
	.previous
#endif
