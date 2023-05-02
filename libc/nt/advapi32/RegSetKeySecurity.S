#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RegSetKeySecurity,RegSetKeySecurity,0

#ifdef __x86_64__
	.text.windows
RegSetKeySecurity:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegSetKeySecurity(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegSetKeySecurity,globl
	.previous
#endif
