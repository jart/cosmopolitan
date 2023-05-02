#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RegConnectRegistryW,RegConnectRegistryW,1608

#ifdef __x86_64__
	.text.windows
RegConnectRegistry:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegConnectRegistryW(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegConnectRegistry,globl
	.previous
#endif
