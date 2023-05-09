#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RegEnumKeyExW,RegEnumKeyExW,0

	.text.windows
RegEnumKeyEx:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegEnumKeyExW(%rip),%rax
	jmp	__sysv2nt8
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	RegEnumKeyEx,globl
	.previous
