#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RegEnumValueW,RegEnumValueW,0

#ifdef __x86_64__
	.text.windows
RegEnumValue:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegEnumValueW(%rip),%rax
	jmp	__sysv2nt8
	.endfn	RegEnumValue,globl
	.previous
#endif
