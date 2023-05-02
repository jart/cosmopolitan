#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RegReplaceKeyW,RegReplaceKeyW,1670

#ifdef __x86_64__
	.text.windows
RegReplaceKey:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegReplaceKeyW(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegReplaceKey,globl
	.previous
#endif
