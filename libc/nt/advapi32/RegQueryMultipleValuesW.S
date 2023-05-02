#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RegQueryMultipleValuesW,RegQueryMultipleValuesW,0

#ifdef __x86_64__
	.text.windows
RegQueryMultipleValues:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegQueryMultipleValuesW(%rip),%rax
	jmp	__sysv2nt6
	.endfn	RegQueryMultipleValues,globl
	.previous
#endif
