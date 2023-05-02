#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RegEnumKeyExW,RegEnumKeyExW,0

#ifdef __x86_64__
	.text.windows
RegEnumKeyEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegEnumKeyExW(%rip),%rax
	jmp	__sysv2nt8
	.endfn	RegEnumKeyEx,globl
	.previous
#endif
