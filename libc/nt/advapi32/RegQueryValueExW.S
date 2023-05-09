#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RegQueryValueExW,RegQueryValueExW,0

	.text.windows
RegQueryValueEx:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegQueryValueExW(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	RegQueryValueEx,globl
	.previous
