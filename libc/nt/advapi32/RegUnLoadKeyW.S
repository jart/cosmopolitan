#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RegUnLoadKeyW,RegUnLoadKeyW,0

#ifdef __x86_64__
	.text.windows
RegUnLoadKey:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegUnLoadKeyW(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegUnLoadKey,globl
	.previous
#endif
