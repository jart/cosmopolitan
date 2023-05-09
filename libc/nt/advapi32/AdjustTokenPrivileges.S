#include "libc/nt/codegen.h"
.imp	advapi32,__imp_AdjustTokenPrivileges,AdjustTokenPrivileges,0

	.text.windows
AdjustTokenPrivileges:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_AdjustTokenPrivileges(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	AdjustTokenPrivileges,globl
	.previous
