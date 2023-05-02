#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RegOpenUserClassesRoot,RegOpenUserClassesRoot,0

#ifdef __x86_64__
	.text.windows
RegOpenUserClassesRoot:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegOpenUserClassesRoot(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegOpenUserClassesRoot,globl
	.previous
#endif
