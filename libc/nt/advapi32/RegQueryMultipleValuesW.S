#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RegQueryMultipleValuesW,RegQueryMultipleValuesW,0

	.text.windows
RegQueryMultipleValues:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegQueryMultipleValuesW(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	RegQueryMultipleValues,globl
	.previous
