#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RegSetValueW,RegSetValueW,1683

	.text.windows
RegSetValue:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegSetValueW(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	RegSetValue,globl
	.previous
