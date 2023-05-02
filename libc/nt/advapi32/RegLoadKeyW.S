#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RegLoadKeyW,RegLoadKeyW,0

#ifdef __x86_64__
	.text.windows
RegLoadKey:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegLoadKeyW(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegLoadKey,globl
	.previous
#endif
