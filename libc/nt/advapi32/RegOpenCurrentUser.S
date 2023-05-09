#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RegOpenCurrentUser,RegOpenCurrentUser,0

	.text.windows
RegOpenCurrentUser:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegOpenCurrentUser(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	RegOpenCurrentUser,globl
	.previous
