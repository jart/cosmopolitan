#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RegDeleteTreeW,RegDeleteTreeW,0

#ifdef __x86_64__
	.text.windows
RegDeleteTree:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegDeleteTreeW(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegDeleteTree,globl
	.previous
#endif
