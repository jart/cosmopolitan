#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RegQueryReflectionKey,RegQueryReflectionKey,1663

#ifdef __x86_64__
	.text.windows
RegQueryReflectionKey:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegQueryReflectionKey(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegQueryReflectionKey,globl
	.previous
#endif
