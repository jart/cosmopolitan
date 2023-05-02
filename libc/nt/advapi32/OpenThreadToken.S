#include "libc/nt/codegen.h"
.imp	advapi32,__imp_OpenThreadToken,OpenThreadToken,0

#ifdef __x86_64__
	.text.windows
OpenThreadToken:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_OpenThreadToken(%rip),%rax
	jmp	__sysv2nt
	.endfn	OpenThreadToken,globl
	.previous
#endif
