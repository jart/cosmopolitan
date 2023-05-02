#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RegEnumKeyW,RegEnumKeyW,1636

#ifdef __x86_64__
	.text.windows
RegEnumKey:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegEnumKeyW(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegEnumKey,globl
	.previous
#endif
