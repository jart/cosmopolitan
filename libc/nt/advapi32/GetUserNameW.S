#include "libc/nt/codegen.h"
.imp	advapi32,__imp_GetUserNameW,GetUserNameW,1381

#ifdef __x86_64__
	.text.windows
GetUserName:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetUserNameW(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetUserName,globl
	.previous
#endif
