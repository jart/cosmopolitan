#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RegQueryValueW,RegQueryValueW,1667

	.text.windows
RegQueryValue:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegQueryValueW(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	RegQueryValue,globl
	.previous
