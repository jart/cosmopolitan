#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RegDeleteValueW,RegDeleteValueW,0

	.text.windows
RegDeleteValue:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegDeleteValueW(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	RegDeleteValue,globl
	.previous
