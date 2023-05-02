#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RegGetKeySecurity,RegGetKeySecurity,0

#ifdef __x86_64__
	.text.windows
RegGetKeySecurity:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegGetKeySecurity(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegGetKeySecurity,globl
	.previous
#endif
