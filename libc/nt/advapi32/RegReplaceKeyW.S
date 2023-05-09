#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RegReplaceKeyW,RegReplaceKeyW,1670

	.text.windows
RegReplaceKey:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegReplaceKeyW(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	RegReplaceKey,globl
	.previous
