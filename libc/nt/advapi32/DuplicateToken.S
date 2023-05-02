#include "libc/nt/codegen.h"
.imp	advapi32,__imp_DuplicateToken,DuplicateToken,0

#ifdef __x86_64__
	.text.windows
DuplicateToken:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_DuplicateToken(%rip),%rax
	jmp	__sysv2nt
	.endfn	DuplicateToken,globl
	.previous
#endif
