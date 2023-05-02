#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RegQueryInfoKeyW,RegQueryInfoKeyW,0

#ifdef __x86_64__
	.text.windows
RegQueryInfoKey:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegQueryInfoKeyW(%rip),%rax
	jmp	__sysv2nt12
	.endfn	RegQueryInfoKey,globl
	.previous
#endif
