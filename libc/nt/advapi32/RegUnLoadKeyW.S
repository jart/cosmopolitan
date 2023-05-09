#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RegUnLoadKeyW,RegUnLoadKeyW,0

	.text.windows
RegUnLoadKey:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegUnLoadKeyW(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	RegUnLoadKey,globl
	.previous
