#include "libc/nt/codegen.h"
.imp	advapi32,__imp_AdjustTokenPrivileges,AdjustTokenPrivileges,0

#ifdef __x86_64__
	.text.windows
AdjustTokenPrivileges:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_AdjustTokenPrivileges(%rip),%rax
	jmp	__sysv2nt6
	.endfn	AdjustTokenPrivileges,globl
	.previous
#endif
