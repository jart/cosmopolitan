#include "libc/nt/codegen.h"
.imp	advapi32,__imp_RegOpenCurrentUser,RegOpenCurrentUser,0

#ifdef __x86_64__
	.text.windows
RegOpenCurrentUser:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegOpenCurrentUser(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegOpenCurrentUser,globl
	.previous
#endif
