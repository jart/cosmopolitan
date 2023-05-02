#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RegCreateKeyW,RegCreateKeyW,1616

#ifdef __x86_64__
	.text.windows
RegCreateKey:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegCreateKeyW(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegCreateKey,globl
	.previous
#endif
