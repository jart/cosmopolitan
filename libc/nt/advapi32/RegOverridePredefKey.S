#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RegOverridePredefKey,RegOverridePredefKey,1658

#ifdef __x86_64__
	.text.windows
RegOverridePredefKey:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegOverridePredefKey(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegOverridePredefKey,globl
	.previous
#endif
